#include "SimulationLogger.h"
#include "Component.h"
#include "Circuit.h"
#include "ComponentFactory.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <sstream>

namespace CircuitSim {

void SimulationLogger::ensureLogFile() {
    if (!fileOpened_) {
        logFile_.open("simulation_log.txt", std::ios::app);
        fileOpened_ = true;
    }
}

void SimulationLogger::log(LogLevel level, const std::string& message) {
    ensureLogFile();
    auto now = std::time(nullptr);
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARN"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::FATAL: levelStr = "FATAL"; break;
    }
    logFile_ << "[" << levelStr << "] " << message << std::endl;
}

void SimulationLogger::logSimulationStep(const std::string& step_description) {
    log(LogLevel::INFO, "Simulation Step: " + step_description);
}

void SimulationLogger::logComponentState(const Component* component) {
    if (!component) return;
    std::string state = "Component State: ID=" + std::to_string(component->getId()) +
                        ", Name=" + component->getName() +
                        ", Type=" + ComponentFactory::typeToString(component->getType());
    log(LogLevel::DEBUG, state);
}

void SimulationLogger::logCircuitState(const Circuit* circuit) {
    if (!circuit) return;
    std::string state = "Circuit State: Components=" + std::to_string(circuit->getComponents().size()) +
                        ", Nodes=" + std::to_string(circuit->getNodes().size()) +
                        ", Branches=" + std::to_string(circuit->getBranches().size()) +
                        ", Solved=" + (circuit->isSolved() ? "Yes" : "No");
    log(LogLevel::INFO, state);
}

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

std::string SimulationLogger::getLog() const {
    std::stringstream ss;
    for (const auto& entry : logs_) {
        ss << "=== Simulation " << entry.simulationId << " ===" << std::endl;
        ss << "Timestamp: " << entry.timestamp << std::endl;
        ss << "Description: " << entry.description << std::endl;
        ss << "Results:" << std::endl;
        for (const auto& result : entry.results) {
            ss << "  ID=" << result.componentId 
               << ", Name=" << result.componentName
               << ", Type=" << result.componentType
               << ", V=" << result.voltage
               << ", I=" << result.current
               << ", P=" << result.power
               << std::endl;
        }
        ss << std::endl;
    }
    return ss.str();
}

} // namespace CircuitSim