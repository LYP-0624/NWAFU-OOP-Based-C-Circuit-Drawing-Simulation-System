cmake_minimum_required(VERSION 3.10)

project(CircuitSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Component.cpp
    src/Resistor.cpp
    src/PowerSource.cpp
    src/Bulb.cpp
    src/Ammeter.cpp
    src/Voltmeter.cpp
    src/Switch.cpp
    src/Port.cpp
    src/Node.cpp
    src/Branch.cpp
    src/Circuit.cpp
    src/LinearSolver.cpp
    src/MNASolver.cpp
    src/ComponentFactory.cpp
    src/Ground.cpp
    src/Capacitor.cpp
    src/Inductor.cpp
    src/CircuitValidator.cpp
    src/SimulationLogger.cpp
)

add_executable(CircuitSim ${SOURCES})

target_compile_features(CircuitSim PRIVATE cxx_std_17)