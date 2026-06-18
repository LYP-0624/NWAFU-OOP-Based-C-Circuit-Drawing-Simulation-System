#ifndef SIMULATION_RESULT_H
#define SIMULATION_RESULT_H

#include <map>
#include <string>

namespace CircuitSim {

struct SimulationResult {
    int componentId;
    std::string componentName;
    std::string componentType;
    double voltage;
    double current;
    double power;
    bool isActive;
    std::map<std::string, double> extra;
};

} // namespace CircuitSim

#endif