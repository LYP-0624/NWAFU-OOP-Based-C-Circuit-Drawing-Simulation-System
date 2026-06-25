#ifndef SIMULATION_LOGGER_H
#define SIMULATION_LOGGER_H

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include "SimulationResult.h"

namespace CircuitSim {

class Component;    // 前向声明：元器件类
class Circuit;      // 前向声明：电路类

// 日志级别枚举
enum class LogLevel {
    DEBUG,      // 调试信息
    INFO,       // 普通信息
    WARNING,    // 警告信息
    ERROR,      // 错误信息
    FATAL       // 致命错误
};

// 仿真日志记录器（单例模式）
class SimulationLogger {
public:
    // 单条日志条目结构体
    struct LogEntry {
        int simulationId;                       // 仿真编号
        double timestamp;                       // 时间戳
        std::vector<SimulationResult> results;  // 仿真结果列表
        std::string description;                // 描述信息
    };

    // 获取单例实例
    static SimulationLogger& getInstance();

    // 记录一次仿真结果
    void logSimulation(const std::vector<SimulationResult>& results, 
                       const std::string& description = "");
    
    // 按级别记录日志
    void log(LogLevel level, const std::string& message);
    void logInfo(const std::string& message) { log(LogLevel::INFO, message); }      // 记录INFO级别
    void logDebug(const std::string& message) { log(LogLevel::DEBUG, message); }    // 记录DEBUG级别
    void logWarn(const std::string& message) { log(LogLevel::WARNING, message); }   // 记录WARNING级别
    void logError(const std::string& message) { log(LogLevel::ERROR, message); }    // 记录ERROR级别
    void logFatal(const std::string& message) { log(LogLevel::FATAL, message); }    // 记录FATAL级别

    // 记录元器件状态
    void logComponentState(const Component* component);
    // 记录电路整体状态
    void logCircuitState(const Circuit* circuit);
    
    // 清空所有日志
    void clear();
    
    // 获取日志总数
    size_t getLogCount() const;
    
    // 获取指定索引的日志条目
    const LogEntry& getLogEntry(int index) const;
    
    // 获取所有日志
    const std::vector<LogEntry>& getAllLogs() const;
    
    // 导出日志为CSV文件
    bool exportCSV(const std::string& filename) const;
    
    // 按元器件ID获取仿真结果
    std::vector<SimulationResult> getResultsById(int componentId) const;
    
    // 获取指定元器件某属性的历史数据
    std::vector<double> getHistory(int componentId, const std::string& property) const;
    
    // 获取日志内容字符串
    std::string getLog() const;

private:
    // 私有构造函数（单例模式）
    SimulationLogger() : nextSimulationId_(1), lastTimestamp_(0.0) {}
    
    int nextSimulationId_;              // 下一个仿真编号
    double lastTimestamp_;              // 上次记录的时间戳
    std::vector<LogEntry> logs_;        // 日志条目列表
    std::ofstream logFile_;             // 日志文件流
    bool fileOpened_ = false;           // 文件是否已打开
    void ensureLogFile();               // 确保日志文件已打开
};

} // namespace CircuitSim

#endif