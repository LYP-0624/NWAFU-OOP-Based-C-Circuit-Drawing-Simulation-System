#include "MNASolver.h"
#include "Port.h"
#include "Resistor.h"
#include "Bulb.h"
#include "Switch.h"
#include "PowerSource.h"
#include "Ground.h"
#include "Capacitor.h"
#include "Inductor.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>

namespace CircuitSim {

MNASolver::MNASolver() : numNodes_(0), numVoltageSources_(0), matrixSize_(0) {}

MNASolver::~MNASolver() = default;

void MNASolver::buildNodeIndexMap(const std::vector<Node*>& nodes) {
    nodeToIndex_.clear();
    nodeOrder_.clear();

    int idx = 0;
    for (auto* node : nodes) {
        if (node && !node->isGround()) {
            nodeToIndex_[node->getId()] = idx++;
            nodeOrder_.push_back(node);
        }
    }

    numNodes_ = idx;
}

void MNASolver::stampResistor(int n1, int n2, double conductance) {
    stampConductance(n1, n2, conductance);
}

void MNASolver::stampVoltageSource(int n1, int n2, int vsIdx, double voltage) {
    int row = numNodes_ + vsIdx;

    if (n1 >= 0 && n1 < numNodes_) {
        matrixA_[row][n1] += 1.0;
        matrixA_[n1][row] += 1.0;
    }
    if (n2 >= 0 && n2 < numNodes_) {
        matrixA_[row][n2] -= 1.0;
        matrixA_[n2][row] -= 1.0;
    }

    vectorB_[row] = voltage;
}

void MNASolver::stampCurrentSource(int n1, int n2, double current) {
    if (n1 >= 0 && n1 < numNodes_) {
        vectorB_[n1] += current;
    }
    if (n2 >= 0 && n2 < numNodes_) {
        vectorB_[n2] -= current;
    }
}

void MNASolver::stampConductance(int n1, int n2, double g) {
    if (n1 >= 0 && n1 < numNodes_) {
        matrixA_[n1][n1] += g;
        if (n2 >= 0 && n2 < numNodes_) {
            matrixA_[n1][n2] -= g;
        }
    }
    if (n2 >= 0 && n2 < numNodes_) {
        matrixA_[n2][n2] += g;
        if (n1 >= 0 && n1 < numNodes_) {
            matrixA_[n2][n1] -= g;
        }
    }
}

void MNASolver::stampCurrentInjection(int node, double current) {
    if (node >= 0 && node < numNodes_) {
        vectorB_[node] += current;
    }
}

bool MNASolver::assembleMatrix(const std::unordered_map<int, Component*>& components,
                               const std::vector<Node*>& nodes) {
    buildNodeIndexMap(nodes);

    numVoltageSources_ = 0;
    for (const auto& pair : components) {
        if (pair.second->getType() == ComponentType::POWER_SOURCE) {
            numVoltageSources_++;
        }
    }

    matrixSize_ = numNodes_ + numVoltageSources_;

    if (matrixSize_ == 0) {
        return false;
    }

    matrixA_.assign(matrixSize_, std::vector<double>(matrixSize_, 0.0));
    vectorB_.assign(matrixSize_, 0.0);
    solutionX_.assign(matrixSize_, 0.0);

    int vsIdx = 0;
    for (const auto& pair : components) {
        Component* comp = pair.second;
        auto& ports = comp->getPorts();
        if (ports.size() < 2) continue;

        int n1 = -1, n2 = -1;
        if (ports[0]->isConnected()) {
            auto it = nodeToIndex_.find(ports[0]->getNode()->getId());
            if (it != nodeToIndex_.end()) n1 = it->second;
        }
        if (ports[1]->isConnected()) {
            auto it = nodeToIndex_.find(ports[1]->getNode()->getId());
            if (it != nodeToIndex_.end()) n2 = it->second;
        }

        switch (comp->getType()) {
            case ComponentType::RESISTOR: {
                double r = static_cast<Resistor*>(comp)->getResistance();
                if (r > 1e-10) {
                    stampResistor(n1, n2, 1.0 / r);
                }
                break;
            }

            case ComponentType::BULB: {
                double r = static_cast<Bulb*>(comp)->getResistance();
                if (r > 1e-10) {
                    stampResistor(n1, n2, 1.0 / r);
                }
                break;
            }

            case ComponentType::SWITCH: {
                bool isOn = static_cast<Switch*>(comp)->isOn();
                double r = isOn ? 0.001 : 1e9;
                if (r > 1e-10 && r < 1e12) {
                    stampResistor(n1, n2, 1.0 / r);
                }
                break;
            }

            case ComponentType::AMMETER: {
                double r = 0.001;
                stampResistor(n1, n2, 1.0 / r);
                break;
            }

            case ComponentType::POWER_SOURCE: {
                double v = static_cast<PowerSource*>(comp)->getValue();
                stampVoltageSource(n1, n2, vsIdx++, v);
                break;
            }

            case ComponentType::VOLTMETER: {
                break;
            }

            case ComponentType::GROUND: {
                break;
            }

            case ComponentType::CAPACITOR: {
                double c = static_cast<Capacitor*>(comp)->getCapacitance();
                double r = 1e12;
                stampResistor(n1, n2, 1.0 / r);
                break;
            }

            case ComponentType::INDUCTOR: {
                double l = static_cast<Inductor*>(comp)->getInductance();
                double r = 0.001;
                stampResistor(n1, n2, 1.0 / r);
                break;
            }

            default:
                break;
        }
    }

    if (numNodes_ > 0) {
        auto it = nodeToIndex_.begin();
        if (it != nodeToIndex_.end()) {
            matrixA_[it->second][it->second] += 1e-12;
        }
    }

    return true;
}

void MNASolver::extractResults(const std::unordered_map<int, Component*>& components,
                               const std::vector<Node*>& nodes) {
    for (size_t i = 0; i < nodeOrder_.size() && i < solutionX_.size(); ++i) {
        nodeOrder_[i]->setVoltage(solutionX_[i]);
    }

    int vsIdx = 0;
    for (const auto& pair : components) {
        Component* comp = pair.second;
        auto& ports = comp->getPorts();
        if (ports.size() < 2) continue;

        double v = 0.0, i = 0.0;

        if (ports[0]->isConnected() && ports[1]->isConnected()) {
            v = ports[0]->getVoltage() - ports[1]->getVoltage();
        }

        switch (comp->getType()) {
            case ComponentType::RESISTOR: {
                double r = static_cast<Resistor*>(comp)->getResistance();
                if (r > 1e-10) {
                    i = v / r;
                }
                break;
            }

            case ComponentType::BULB: {
                double r = static_cast<Bulb*>(comp)->getResistance();
                if (r > 1e-10) {
                    i = v / r;
                }
                break;
            }

            case ComponentType::SWITCH: {
                bool isOn = static_cast<Switch*>(comp)->isOn();
                double r = isOn ? 0.001 : 1e9;
                if (r > 1e-10 && r < 1e12) {
                    i = v / r;
                }
                break;
            }

            case ComponentType::AMMETER: {
                if (vsIdx < numVoltageSources_) {
                    i = solutionX_[numNodes_ + vsIdx];
                }
                vsIdx++;
                break;
            }

            case ComponentType::POWER_SOURCE: {
                if (vsIdx < numVoltageSources_) {
                    i = solutionX_[numNodes_ + vsIdx];
                }
                vsIdx++;
                break;
            }

            case ComponentType::VOLTMETER: {
                i = 0.0;
                break;
            }

            case ComponentType::GROUND: {
                v = 0.0;
                break;
            }

            case ComponentType::CAPACITOR: {
                double c = static_cast<Capacitor*>(comp)->getCapacitance();
                double r = 1e12;
                if (r > 1e-10) {
                    i = v / r;
                }
                break;
            }

            case ComponentType::INDUCTOR: {
                double l = static_cast<Inductor*>(comp)->getInductance();
                double r = 0.001;
                if (r > 1e-10) {
                    i = v / r;
                }
                break;
            }

            default:
                break;
        }

        comp->simulate(v, i);
    }
}

bool MNASolver::solve(const std::unordered_map<int, Component*>& components,
                      const std::vector<Node*>& nodes) {
    if (components.empty() || nodes.empty()) {
        return false;
    }

    std::string errorMsg;
    if (!validateCircuit(components, nodes, errorMsg)) {
        return false;
    }

    if (!assembleMatrix(components, nodes)) {
        return false;
    }

    SolverStats stats;
    if (!LinearSolver::solveWithStats(matrixA_, vectorB_, solutionX_, stats)) {
        return false;
    }

    extractResults(components, nodes);
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

    bool hasSource = false;
    for (const auto& pair : components) {
        if (pair.second->getType() == ComponentType::POWER_SOURCE) {
            hasSource = true;
            break;
        }
    }

    if (!hasSource) {
        errorMsg = "No voltage source in circuit";
        return false;
    }

    std::set<int> connectedNodes;
    for (const auto& pair : components) {
        for (auto* port : pair.second->getPorts()) {
            if (port->isConnected()) {
                connectedNodes.insert(port->getNode()->getId());
            }
        }
    }

    if (connectedNodes.size() < 2) {
        errorMsg = "Circuit is not properly connected";
        return false;
    }

    return true;
}

} // namespace CircuitSim