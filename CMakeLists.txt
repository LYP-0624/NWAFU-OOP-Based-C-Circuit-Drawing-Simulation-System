cmake_minimum_required(VERSION 3.20)
project(CircuitSim)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    main.cpp
    Component.cpp
    Port.cpp
    Node.cpp
    Circuit.cpp
    LinearSolver.cpp
    MNASolver.cpp
    ComponentFactory.cpp
    Resistor.cpp
    PowerSource.cpp
    Bulb.cpp
    Ammeter.cpp
    Voltmeter.cpp
    Switch.cpp
)

add_executable(CircuitSim ${SOURCES})