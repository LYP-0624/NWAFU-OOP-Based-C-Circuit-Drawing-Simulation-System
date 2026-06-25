#ifndef MNA_SOLVER_H
#define MNA_SOLVER_H

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "Component.h"
#include "LinearSolver.h"
#include "Node.h"

namespace CircuitSim {

// MNA模板类型
enum class StampType {
    RESISTOR,      // 电阻
    VOLTAGE_SOURCE, // 电压源
    CURRENT_SOURCE, // 电流源
    CCCS,          // 电流控制电流源
    CCVS,          // 电流控制电压源
    VCCS,          // 电压控制电流源
    VCVS           // 电压控制电压源
};

// MNA模板结构
struct MNAStamp {
    StampType type;           // 模板类型
    int nodePlus;             // 正极节点索引
    int nodeMinus;            // 负极节点索引
    int voltageSourceIdx;     // 电压源索引
    double value;             // 主值
    double value2;            // 辅值
};

// Modified Nodal Analysis 求解器
class MNASolver {
private:
    std::map<int, int> nodeToIndex_;           // 节点ID到矩阵索引的映射
    std::vector<Node*> nodeOrder_;             // 节点顺序
    Node* groundNode_;                         // 地节点
    int numNodes_;                             // 节点数量(不含地)
    int numVoltageSources_;                    // 电压源数量
    int matrixSize_;                           // 矩阵尺寸
    std::string lastError_;                    // 最后错误信息

    std::vector<std::vector<double>> matrixA_; // MNA系数矩阵
    std::vector<double> vectorB_;              // 右端向量
    std::vector<double> solutionX_;            // 解向量
    std::vector<const Component*> voltageSourceOrder_; // 电压源顺序

    void buildNodeIndexMap(const std::vector<Node*>& nodes); // 建立节点编号映射
    void stampResistor(int n1, int n2, double conductance); // 电阻模板
    void stampVoltageSource(int n1, int n2, int vsIdx, double voltage); // 电压源模板
    void stampCurrentSource(int n1, int n2, double current); // 电流源模板
    void stampConductance(int n1, int n2, double g);         // 电导模板
    void stampCurrentInjection(int node, double current);    // 电流注入

    bool assembleMatrix(const std::unordered_map<int, Component*>& components,
                        const std::vector<Node*>& nodes);     // 构建导纳矩阵
    void extractResults(const std::unordered_map<int, Component*>& components,
                        const std::vector<Node*>& nodes);     // 提取计算结果

public:
    MNASolver();
    ~MNASolver();

    bool solve(const std::unordered_map<int, Component*>& components,
               const std::vector<Node*>& nodes); // 执行MNA求解

    const std::vector<double>& getSolution() const { return solutionX_; }
    int getMatrixSize() const { return matrixSize_; }
    int getNumVoltageSources() const { return numVoltageSources_; }
    std::string getLastError() const { return lastError_; }

    double getNodeVoltage(int nodeIdx) const;           // 获取节点电压
    double getVoltageSourceCurrent(int vsIdx) const;    // 获取电压源电流

    static bool validateCircuit(const std::unordered_map<int, Component*>& components,
                                const std::vector<Node*>& nodes,
                                std::string& errorMsg); // 验证电路
};

} // namespace CircuitSim

#endif
