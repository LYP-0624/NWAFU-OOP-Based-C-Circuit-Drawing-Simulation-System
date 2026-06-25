#include "MNASolver.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_set>

#include "Ammeter.h"
#include "Bulb.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Switch.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Voltmeter.h"
#include "Port.h"

namespace CircuitSim {

namespace {

// 按ID排序元件
std::vector<const Component*> orderedComponents(const std::unordered_map<int, Component*>& components) {
    std::vector<const Component*> ordered;
    ordered.reserve(components.size());
    for (const auto& [id, comp] : components) {
        ordered.push_back(comp);
    }
    std::sort(ordered.begin(), ordered.end(), [](const Component* a, const Component* b) {
        return a->getId() < b->getId();
    });
    return ordered;
}

// 获取元件等效电阻
double componentResistance(const Component* comp) {
    switch (comp->getType()) {
        case ComponentType::RESISTOR:
            return std::max(1e-12, static_cast<const Resistor*>(comp)->getResistance());
        case ComponentType::BULB:
            return std::max(1e-12, static_cast<const Bulb*>(comp)->getResistance());
        case ComponentType::AMMETER:
            return 1e-3;  // 电流表内阻
        case ComponentType::VOLTMETER:
            return 1e9;   // 电压表内阻
        case ComponentType::SWITCH:
            return static_cast<const Switch*>(comp)->isOn()
                ? std::max(1e-12, static_cast<const Switch*>(comp)->getResistanceOn())
                : std::max(1e-12, static_cast<const Switch*>(comp)->getResistanceOff());
        case ComponentType::CAPACITOR:
            return 1e12;  // 电容直流开路
        case ComponentType::INDUCTOR:
            return 1e-6;  // 电感直流短路近似
        case ComponentType::POWER_SOURCE:
            return 0.0;
        default:
            return 1e9;
    }
}

} // namespace

MNASolver::MNASolver()
    : groundNode_(nullptr), numNodes_(0), numVoltageSources_(0), matrixSize_(0) {}

MNASolver::~MNASolver() = default;

// 建立节点编号映射
void MNASolver::buildNodeIndexMap(const std::vector<Node*>& nodes) {
    nodeToIndex_.clear();
    nodeOrder_.clear();
    groundNode_ = nullptr;

    // 查找地节点
    for (auto* node : nodes) {
        if (node && node->isGround()) {
            groundNode_ = node;
            break;
        }
    }

    // 如果没有地节点，使用第一个节点作为参考节点
    if (!groundNode_ && !nodes.empty()) {
        groundNode_ = nodes.front();
    }

    // 为非地节点分配矩阵索引
    int idx = 0;
    for (auto* node : nodes) {
        if (!node || node == groundNode_) {
            continue;
        }
        nodeToIndex_[node->getId()] = idx++;
        nodeOrder_.push_back(node);
    }

    numNodes_ = idx;
}

// 电导模板
void MNASolver::stampConductance(int n1, int n2, double g) {
    if (g == 0.0) {
        return;
    }
    if (n1 >= 0) {
        matrixA_[n1][n1] += g;
        if (n2 >= 0) {
            matrixA_[n1][n2] -= g;
        }
    }
    if (n2 >= 0) {
        matrixA_[n2][n2] += g;
        if (n1 >= 0) {
            matrixA_[n2][n1] -= g;
        }
    }
}

void MNASolver::stampResistor(int n1, int n2, double conductance) {
    stampConductance(n1, n2, conductance);
}

// 电压源模板(KCL约束)
void MNASolver::stampVoltageSource(int n1, int n2, int vsIdx, double voltage) {
    const int row = numNodes_ + vsIdx;

    if (n1 >= 0) {
        matrixA_[n1][row] += 1.0;
        matrixA_[row][n1] += 1.0;
    }
    if (n2 >= 0) {
        matrixA_[n2][row] -= 1.0;
        matrixA_[row][n2] -= 1.0;
    }

    vectorB_[row] += voltage;
}

// 电流源模板
void MNASolver::stampCurrentSource(int n1, int n2, double current) {
    if (n1 >= 0) {
        vectorB_[n1] -= current;
    }
    if (n2 >= 0) {
        vectorB_[n2] += current;
    }
}

void MNASolver::stampCurrentInjection(int node, double current) {
    if (node >= 0) {
        vectorB_[node] += current;
    }
}

// 构建导纳矩阵
bool MNASolver::assembleMatrix(const std::unordered_map<int, Component*>& components,
                               const std::vector<Node*>& nodes) {
    buildNodeIndexMap(nodes);

    // 收集电压源
    voltageSourceOrder_.clear();
    const auto ordered = orderedComponents(components);
    for (const Component* comp : ordered) {
        if (comp->getType() == ComponentType::POWER_SOURCE) {
            const auto* source = static_cast<const PowerSource*>(comp);
            if (source->isVoltageSource()) {
                voltageSourceOrder_.push_back(comp);
            }
        }
    }

    numVoltageSources_ = static_cast<int>(voltageSourceOrder_.size());
    matrixSize_ = numNodes_ + numVoltageSources_;
    if (matrixSize_ == 0) {
        lastError_ = "Circuit has no solvable nodes";
        return false;
    }

    // 初始化矩阵和向量
    matrixA_.assign(matrixSize_, std::vector<double>(matrixSize_, 0.0));
    vectorB_.assign(matrixSize_, 0.0);
    solutionX_.assign(matrixSize_, 0.0);

    // 遍历所有元件并添加到矩阵
    int vsIdx = 0;
    for (const Component* comp : ordered) {
        const auto ports = comp->getPorts();
        if (ports.size() < 2 || !ports[0] || !ports[1] || !ports[0]->isConnected() || !ports[1]->isConnected()) {
            continue;
        }

        // 获取端口对应的节点索引
        auto nodeIndexOf = [this](const Port* port) -> int {
            if (!port || !port->isConnected() || !port->getNode()) {
                return -1;
            }
            if (port->getNode() == groundNode_) {
                return -1;
            }
            auto it = nodeToIndex_.find(port->getNode()->getId());
            return it == nodeToIndex_.end() ? -1 : it->second;
        };

        const int n1 = nodeIndexOf(ports[0]);
        const int n2 = nodeIndexOf(ports[1]);

        // 根据元件类型进行模板处理
        switch (comp->getType()) {
            case ComponentType::RESISTOR:
            case ComponentType::BULB:
            case ComponentType::SWITCH:
            case ComponentType::AMMETER:
            case ComponentType::VOLTMETER:
            case ComponentType::CAPACITOR:
            case ComponentType::INDUCTOR: {
                const double r = componentResistance(comp);
                if (r > 0.0) {
                    stampResistor(n1, n2, 1.0 / r);
                }
                break;
            }
            case ComponentType::POWER_SOURCE: {
                const auto* source = static_cast<const PowerSource*>(comp);
                if (source->isVoltageSource()) {
                    stampVoltageSource(n1, n2, vsIdx++, source->getValue());
                } else {
                    stampCurrentSource(n1, n2, source->getValue());
                }
                break;
            }
            default:
                break;
        }
    }

    // 添加微小电导防止矩阵奇异
    if (numNodes_ > 0) {
        matrixA_[0][0] += 1e-12;
    }

    return true;
}

// 提取计算结果
void MNASolver::extractResults(const std::unordered_map<int, Component*>& components,
                               const std::vector<Node*>& nodes) {
    // 地节点电压设为0
    if (groundNode_) {
        groundNode_->setVoltage(0.0);
    }

    // 设置各节点电压
    for (size_t i = 0; i < nodeOrder_.size() && i < solutionX_.size(); ++i) {
        nodeOrder_[i]->setVoltage(solutionX_[i]);
    }

    // 计算各元件的电压和电流
    const auto ordered = orderedComponents(components);
    int sourceIndex = 0;
    for (const Component* comp : ordered) {
        const auto ports = comp->getPorts();
        double voltage = 0.0;
        double current = 0.0;

        // 计算元件两端电压
        if (ports.size() >= 2 && ports[0] && ports[1] && ports[0]->isConnected() && ports[1]->isConnected()) {
            voltage = ports[0]->getVoltage() - ports[1]->getVoltage();
        }

        // 根据元件类型计算电流
        switch (comp->getType()) {
            case ComponentType::RESISTOR: {
                const double r = std::max(1e-12, static_cast<const Resistor*>(comp)->getResistance());
                current = voltage / r;
                break;
            }
            case ComponentType::BULB: {
                const double r = std::max(1e-12, static_cast<const Bulb*>(comp)->getResistance());
                current = voltage / r;
                break;
            }
            case ComponentType::SWITCH: {
                const auto* sw = static_cast<const Switch*>(comp);
                const double r = sw->isOn() ? std::max(1e-12, sw->getResistanceOn())
                                            : std::max(1e-12, sw->getResistanceOff());
                current = voltage / r;
                break;
            }
            case ComponentType::AMMETER: {
                current = voltage / 1e-3;
                break;
            }
            case ComponentType::VOLTMETER: {
                current = voltage / 1e9;
                break;
            }
            case ComponentType::CAPACITOR: {
                current = 0.0;
                break;
            }
            case ComponentType::INDUCTOR: {
                current = voltage / 1e-6;
                break;
            }
            case ComponentType::POWER_SOURCE: {
                const auto* source = static_cast<const PowerSource*>(comp);
                if (source->isVoltageSource()) {
                    // 电压源电流从解向量获取
                    if (sourceIndex < numVoltageSources_) {
                        current = solutionX_[numNodes_ + sourceIndex];
                    }
                    ++sourceIndex;
                } else {
                    current = source->getValue();
                }
                break;
            }
            default:
                break;
        }

        // 更新元件仿真结果
        const_cast<Component*>(comp)->simulate(voltage, current);
    }
}

// 执行MNA求解
bool MNASolver::solve(const std::unordered_map<int, Component*>& components,
                      const std::vector<Node*>& nodes) {
    if (components.empty()) {
        lastError_ = "No components in circuit";
        return false;
    }
    if (nodes.empty()) {
        lastError_ = "No nodes in circuit";
        return false;
    }

    // 验证电路
    std::string errorMsg;
    if (!validateCircuit(components, nodes, errorMsg)) {
        lastError_ = errorMsg;
        return false;
    }

    // 构建矩阵
    if (!assembleMatrix(components, nodes)) {
        if (lastError_.empty()) {
            lastError_ = "Matrix assembly failed";
        }
        return false;
    }

    // 求解线性方程组
    SolverStats stats;
    if (!LinearSolver::solveWithStats(matrixA_, vectorB_, solutionX_, stats)) {
        lastError_ = stats.errorMessage.empty() ? "Linear solve failed" : stats.errorMessage;
        return false;
    }

    // 提取结果
    extractResults(components, nodes);
    lastError_.clear();
    return true;
}

double MNASolver::getNodeVoltage(int nodeIdx) const {
    if (nodeIdx >= 0 && nodeIdx < numNodes_) {
        return solutionX_[nodeIdx];
    }
    return 0.0;
}

double MNASolver::getVoltageSourceCurrent(int vsIdx) const {
    if (vsIdx >= 0 && vsIdx < numVoltageSources_) {
        return solutionX_[numNodes_ + vsIdx];
    }
    return 0.0;
}

// 验证电路是否可求解
bool MNASolver::validateCircuit(const std::unordered_map<int, Component*>& components,
                                const std::vector<Node*>& nodes,
                                std::string& errorMsg) {
    if (components.empty()) {
        errorMsg = "No components in circuit";
        return false;
    }
    if (nodes.empty()) {
        errorMsg = "No nodes in circuit";
        return false;
    }

    // 检查是否有电源
    bool hasSource = false;
    for (const auto& [id, comp] : components) {
        if (comp->getType() == ComponentType::POWER_SOURCE) {
            hasSource = true;
            break;
        }
    }
    if (!hasSource) {
        errorMsg = "Circuit requires at least one power source";
        return false;
    }

    // 检查所有元件是否正确连接
    for (const auto& [id, comp] : components) {
        const auto ports = comp->getPorts();
        if (ports.size() < 2 || !ports[0] || !ports[1] || !ports[0]->isConnected() || !ports[1]->isConnected()) {
            errorMsg = "Component " + std::to_string(id) + " is not fully connected";
            return false;
        }
        if (ports[0]->getNode() == ports[1]->getNode()) {
            errorMsg = "Component " + std::to_string(id) + " is shorted on the same node";
            return false;
        }
    }

    return true;
}

} // namespace CircuitSim
