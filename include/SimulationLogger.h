#ifndef SIMULATION_LOGGER_H
#define SIMULATION_LOGGER_H

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include "SimulationResult.h"

namespace CircuitSim {

class Component;
class Circuit;

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

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
    
    void log(LogLevel level, const std::string& message);
    void logInfo(const std::string& message) { log(LogLevel::INFO, message); }
    void logDebug(const std::string& message) { log(LogLevel::DEBUG, message); }
    void logWarn(const std::string& message) { log(LogLevel::WARNING, message); }
    void logError(const std::string& message) { log(LogLevel::ERROR, message); }
    void logFatal(const std::string& message) { log(LogLevel::FATAL, message); }

    void logComponentState(const Component* component);
    void logCircuitState(const Circuit* circuit);
    
    void clear();
    
    size_t getLogCount() const;
    
    const LogEntry& getLogEntry(int index) const;
    
    const std::vector<LogEntry>& getAllLogs() const;
    
    bool exportCSV(const std::string& filename) const;
    
    std::vector<SimulationResult> getResultsById(int componentId) const;
    
    std::vector<double> getHistory(int componentId, const std::string& property) const;
    
    std::string getLog() const;

private:
    SimulationLogger() : nextSimulationId_(1), lastTimestamp_(0.0) {}
    
    int nextSimulationId_;
    double lastTimestamp_;
    std::vector<LogEntry> logs_;
    std::ofstream logFile_;
    bool fileOpened_ = false;
    void ensureLogFile();
};

} // namespace CircuitSim

#endif
