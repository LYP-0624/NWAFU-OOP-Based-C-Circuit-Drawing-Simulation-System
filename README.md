Simple Circuit Diagram Generation & Simulation System (C++ Qt OOP Internship Project)
Overview
This C++ Qt project is an object-oriented circuit drawing and real-time simulation platform completed for engineering practice. It enables visual circuit construction and accurate electrical numerical calculation for basic analog circuits.
Core Functions
Graphical Interactive Canvas
Built with Qt GUI library, users drag-and-drop electronic components (resistor, power supply, bulb, ammeter, voltmeter, switch, capacitor, inductor) on canvas, draw wires with pin snap-to-fit, and adjust component positions freely. Dynamic visual effects include lit bulbs and real-time meter readings linked to simulation data.
OOP Component Hierarchy Design
Adopts inheritance and polymorphism: an abstract base class Component defines unified virtual interfaces; all specific circuit elements inherit and rewrite electrical simulation logic. The Factory Pattern dynamically generates component instances, and UML class diagrams standardize relationships among all classes.
Circuit Topology Management
The core Circuit class manages circuit nodes, ports and branches via graph structure (adjacency list). It supports auto node merging, branch identification, loop detection and connection/disconnection operations.
Physics Simulation Engine Based on Circuit Laws
Implements Modified Nodal Analysis (MNA) combined with Gaussian elimination solver. It establishes linear equations following KCL & KVL to compute real-time node voltage and branch current, supporting series, parallel and hybrid circuit analysis. Built-in circuit validator checks ground nodes, connectivity and short-circuit faults.
Data Persistence & Logging
Serializes/deserializes circuit layout data in JSON format to create, save and reload circuit files. The singleton logger records full simulation procedures and exports running data as CSV for review and statistics.
Technical Architecture
Language: C++
GUI Framework: Qt
Programming Paradigm: Object-Oriented Programming (Inheritance, Polymorphism, Factory & Singleton Patterns)
Circuit Algorithm: MNA, Gaussian Elimination with partial pivoting
Data Storage: JSON serialization
Testing: Unit tests for voltage division and current shunt verification
Project Modules
Component Class System & UML Architecture
Circuit Topology Modeling (Circuit / Node / Port / Branch)
MNA Simulation Solver & Linear Matrix Calculator
Qt Graphical Interactive Drawing Module
JSON Persistence, Circuit Validation & Simulation Logging
Use Cases
Suitable for basic circuit teaching demonstration, student circuit simulation practice, and lightweight analog DC circuit numerical calculation.
