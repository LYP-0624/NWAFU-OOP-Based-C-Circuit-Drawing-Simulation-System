// 新增日志级别方法
void logDebug(const std::string& message);
void logWarn(const std::string& message);
void logError(const std::string& message);
void logFatal(const std::string& message);

// 新增仿真过程日志
void logSimulationStep(const std::string& step_description);
void logComponentState(const Component* component);
void logCircuitState(const Circuit* circuit);
