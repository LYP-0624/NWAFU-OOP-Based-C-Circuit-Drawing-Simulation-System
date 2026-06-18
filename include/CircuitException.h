#ifndef CIRCUIT_EXCEPTION_H
#define CIRCUIT_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace CircuitSim {

class CircuitException : public std::runtime_error {
public:
    explicit CircuitException(const std::string& message) 
        : std::runtime_error(message) {}
};

class InvalidConnectionException : public CircuitException {
public:
    explicit InvalidConnectionException(const std::string& message) 
        : CircuitException("Invalid connection: " + message) {}
};

class OpenCircuitException : public CircuitException {
public:
    explicit OpenCircuitException(const std::string& message = "Open circuit detected") 
        : CircuitException(message) {}
};

class SingularMatrixException : public CircuitException {
public:
    explicit SingularMatrixException(const std::string& message = "Singular matrix encountered") 
        : CircuitException(message) {}
};

class NoGroundException : public CircuitException {
public:
    explicit NoGroundException(const std::string& message = "No ground node found") 
        : CircuitException(message) {}
};

class InvalidJsonException : public CircuitException {
public:
    explicit InvalidJsonException(const std::string& message = "Invalid JSON format") 
        : CircuitException("Invalid JSON: " + message) {}
};

class FileIOException : public CircuitException {
public:
    explicit FileIOException(const std::string& message = "File I/O error") 
        : CircuitException("File I/O: " + message) {}
};

} // namespace CircuitSim

#endif