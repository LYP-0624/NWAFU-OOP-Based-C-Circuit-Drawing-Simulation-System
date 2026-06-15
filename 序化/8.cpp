// 新增日志方法实现
void SimulationLogger::logDebug(const std::string& message)
{
    log(LogLevel::DEBUG, message);
}

void SimulationLogger::logWarn(const std::string& message)
{
    log(LogLevel::WARNING, message);
}

void SimulationLogger::logError(const std::string& message)
{
    log(LogLevel::ERROR, message);
}

void SimulationLogger::logFatal(const std::string& message)
{
    log(LogLevel::FATAL, message);
}

void SimulationLogger::logSimulationStep(const std::string& step_description)
{
    log(LogLevel::INFO, "Simulation Step: " + step_description);
}

void SimulationLogger::logComponentState(const Component* component)
{
    if (!component) return;
    std::string state = "Component State: ID=" + std::to_string(component->getId()) +
                        ", Type=" + component->getType() +
                        ", Name=" + component->getName() +
                        ", Voltage=" + std::to_string(component->getVoltage()) +
                        ", Current=" + std::to_string(component->getCurrent());
    log(LogLevel::DEBUG, state);
}

void SimulationLogger::logCircuitState(const Circuit* circuit)
{
    if (!circuit) return;
    std::string state = "Circuit State: Components=" + std::to_string(circuit->getComponents().size()) +
                        ", Nodes=" + std::to_string(circuit->getNodes().size()) +
                        ", Branches=" + std::to_string(circuit->getBranches().size()) +
                        ", Solved=" + (circuit->isSolved() ? "Yes" : "No");
    log(LogLevel::INFO, state);
}
