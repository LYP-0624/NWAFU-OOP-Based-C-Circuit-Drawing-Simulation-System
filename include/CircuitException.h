#ifndef CIRCUIT_EXCEPTION_H
#define CIRCUIT_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace CircuitSim {

// 电路仿真基础异常类，继承自标准运行时错误
class CircuitException : public std::runtime_error {
public:
    // 构造函数，接收错误信息字符串
    explicit CircuitException(const std::string& message) 
        : std::runtime_error(message) {}
};

// 无效连接异常，如将元件接到不存在的节点
class InvalidConnectionException : public CircuitException {
public:
    // 构造函数，在传入信息前加上"Invalid connection: "前缀
    explicit InvalidConnectionException(const std::string& message) 
        : CircuitException("Invalid connection: " + message) {}
};

// 开路异常，电路中存在断开路径
class OpenCircuitException : public CircuitException {
public:
    // 默认错误信息为"Open circuit detected"
    explicit OpenCircuitException(const std::string& message = "Open circuit detected") 
        : CircuitException(message) {}
};

// 奇异矩阵异常，电路方程组无唯一解（如短路导致）
class SingularMatrixException : public CircuitException {
public:
    // 默认错误信息为"Singular matrix encountered"
    explicit SingularMatrixException(const std::string& message = "Singular matrix encountered") 
        : CircuitException(message) {}
};

// 无接地节点异常，电路缺少参考地
class NoGroundException : public CircuitException {
public:
    // 默认错误信息为"No ground node found"
    explicit NoGroundException(const std::string& message = "No ground node found") 
        : CircuitException(message) {}
};

// JSON格式无效异常，解析电路文件时出错
class InvalidJsonException : public CircuitException {
public:
    // 默认错误信息为"Invalid JSON: " + 传入信息
    explicit InvalidJsonException(const std::string& message = "Invalid JSON format") 
        : CircuitException("Invalid JSON: " + message) {}
};

// 文件读写异常，打开或保存电路文件失败
class FileIOException : public CircuitException {
public:
    // 默认错误信息为"File I/O: " + 传入信息
    explicit FileIOException(const std::string& message = "File I/O error") 
        : CircuitException("File I/O: " + message) {}
};

} // namespace CircuitSim

#endif