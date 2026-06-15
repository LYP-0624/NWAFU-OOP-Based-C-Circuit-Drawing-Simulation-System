#ifndef MNA_SOLVER_H
#define MNA_SOLVER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "Component.h"
#include "Port.h"
#include "Node.h"
#include "LinearSolver.h"

namespace CircuitSim {

enum class StampType {
    RESISTOR,
    VOLTAGE_SOURCE,
    CURRENT_SOURCE,
    CCCS,
    CCVS,
    VCCS,
    VCVS
};

struct MNAStamp {
    StampType type;
    int nodePlus;
    int nodeMinus;
    int voltageSourceIdx;
    double value;
    double value2;
};

class MNASolver {
private:
    std::map<int, int> nodeToIndex_;
    std::vector<Node*> nodeOrder_;
    int numNodes_;
    int numVoltageSources_;
    int matrixSize_;

    std::vector<std::vector<double>> matrixA_;
    std::vector<double> vectorB_;
    std::vector<double> solutionX_;

    void buildNodeIndexMap(const std::vector<Node*>& nodes);
    void stampResistor(int n1, int n2, double conductance);
    void stampVoltageSource(int n1, int n2, int vsIdx, double voltage);
    void stampCurrentSource(int n1, int n2, double current);
    void stampConductance(int n1, int n2, double g);
    void stampCurrentInjection(int node, double current);

    bool assembleMatrix(const std::unordered_map<int, Component*>& components,
                       const std::vector<Node*>& nodes);
    void extractResults(const std::unordered_map<int, Component*>& components,
                       const std::vector<Node*>& nodes);

public:
    MNASolver();
    ~MNASolver();

    bool solve(const std::unordered_map<int, Component*>& components,
               const std::vector<Node*>& nodes);

    const std::vector<double>& getSolution() const { return solutionX_; }
    int getMatrixSize() const { return matrixSize_; }
    int getNumVoltageSources() const { return numVoltageSources_; }

    double getNodeVoltage(int nodeIdx) const;
    double getVoltageSourceCurrent(int vsIdx) const;

    static bool validateCircuit(const std::unordered_map<int, Component*>& components,
                               const std::vector<Node*>& nodes,
                               std::string& errorMsg);
};

} // namespace CircuitSim

#endif