#ifndef SIMULATION_LOGGER_H
#define SIMULATION_LOGGER_H

#include <vector>
#include <string>
#include <map>
#include "SimulationResult.h"

namespace CircuitSim {

class SimulationLogger {
public:
    struct LogEntry {
        int simulationId;
        double timestamp;
        std::vector<SimulationResult> results;
        std::string description;
    };

    static SimulationLogger& getInstance();

    void logSimulation(const std::vector<SimulationResult>& results, 
                       const std::string& description = "");
    
    void clear();
    
    size_t getLogCount() const;
    
    const LogEntry& getLogEntry(int index) const;
    
    const std::vector<LogEntry>& getAllLogs() const;
    
    bool exportCSV(const std::string& filename) const;
    
    std::vector<SimulationResult> getResultsById(int componentId) const;
    
    std::vector<double> getHistory(int componentId, const std::string& property) const;

private:
    SimulationLogger() : nextSimulationId_(1), lastTimestamp_(0.0) {}
    
    int nextSimulationId_;
    double lastTimestamp_;
    std::vector<LogEntry> logs_;
};

} // namespace CircuitSim

#endif