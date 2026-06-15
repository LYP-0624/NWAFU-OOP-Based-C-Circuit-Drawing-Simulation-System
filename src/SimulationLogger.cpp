#include "SimulationLogger.h"
#include <fstream>
#include <iomanip>

namespace CircuitSim {

SimulationLogger& SimulationLogger::getInstance() {
    static SimulationLogger instance;
    return instance;
}

void SimulationLogger::logSimulation(const std::vector<SimulationResult>& results,
                                     const std::string& description) {
    LogEntry entry;
    entry.simulationId = nextSimulationId_++;
    entry.timestamp = lastTimestamp_ += 1.0;
    entry.results = results;
    entry.description = description;
    logs_.push_back(entry);
}

void SimulationLogger::clear() {
    logs_.clear();
    nextSimulationId_ = 1;
    lastTimestamp_ = 0.0;
}

size_t SimulationLogger::getLogCount() const {
    return logs_.size();
}

const SimulationLogger::LogEntry& SimulationLogger::getLogEntry(int index) const {
    static LogEntry empty;
    if (index >= 0 && static_cast<size_t>(index) < logs_.size()) {
        return logs_[index];
    }
    return empty;
}

const std::vector<SimulationLogger::LogEntry>& SimulationLogger::getAllLogs() const {
    return logs_;
}

bool SimulationLogger::exportCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "SimulationID,Timestamp,ComponentID,ComponentName,ComponentType,"
         << "Voltage,Current,Power,IsActive";
    
    file << "\n";
    
    for (const auto& entry : logs_) {
        for (const auto& result : entry.results) {
            file << entry.simulationId << ","
                 << std::fixed << std::setprecision(6) << entry.timestamp << ","
                 << result.componentId << ","
                 << "\"" << result.componentName << "\","
                 << "\"" << result.componentType << "\","
                 << std::fixed << std::setprecision(6) << result.voltage << ","
                 << std::fixed << std::setprecision(6) << result.current << ","
                 << std::fixed << std::setprecision(6) << result.power << ","
                 << (result.isActive ? "1" : "0") << "\n";
        }
    }
    
    file.close();
    return true;
}

std::vector<SimulationResult> SimulationLogger::getResultsById(int componentId) const {
    std::vector<SimulationResult> results;
    for (const auto& entry : logs_) {
        for (const auto& result : entry.results) {
            if (result.componentId == componentId) {
                results.push_back(result);
            }
        }
    }
    return results;
}

std::vector<double> SimulationLogger::getHistory(int componentId, const std::string& property) const {
    std::vector<double> history;
    for (const auto& entry : logs_) {
        for (const auto& result : entry.results) {
            if (result.componentId == componentId) {
                if (property == "voltage") {
                    history.push_back(result.voltage);
                } else if (property == "current") {
                    history.push_back(result.current);
                } else if (property == "power") {
                    history.push_back(result.power);
                } else if (result.extra.count(property)) {
                    history.push_back(result.extra.at(property));
                } else {
                    history.push_back(0.0);
                }
            }
        }
    }
    return history;
}

} // namespace CircuitSim