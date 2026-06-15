#include <iostream>
#include <iomanip>
#include "../include/Circuit.h"
#include "../include/Resistor.h"
#include "../include/PowerSource.h"
#include "../include/Bulb.h"
#include "../include/Switch.h"
#include "../include/Ground.h"
#include "../include/Capacitor.h"
#include "../include/Inductor.h"
#include "../include/CircuitValidator.h"
#include "../include/SimulationLogger.h"

void printResults(const std::vector<CircuitSim::SimulationResult>& results) {
    std::cout << "\n=== Simulation Results ===\n";
    std::cout << std::setw(10) << "ID" 
              << std::setw(15) << "Name" 
              << std::setw(12) << "Type"
              << std::setw(12) << "Voltage(V)"
              << std::setw(12) << "Current(A)"
              << std::setw(12) << "Power(W)"
              << std::setw(10) << "Active"
              << std::endl;
    std::cout << "-------------------------------------------------------------------------\n";
    
    for (const auto& r : results) {
        std::cout << std::setw(10) << r.componentId
                  << std::setw(15) << r.componentName
                  << std::setw(12) << r.componentType
                  << std::setw(12) << std::fixed << std::setprecision(4) << r.voltage
                  << std::setw(12) << std::fixed << std::setprecision(6) << r.current
                  << std::setw(12) << std::fixed << std::setprecision(4) << r.power
                  << std::setw(10) << (r.isActive ? "Yes" : "No")
                  << std::endl;
    }
}

void testBasicCircuit() {
    std::cout << "\n=== Test 1: Basic Series Circuit ===" << std::endl;
    
    CircuitSim::Circuit circuit;
    
    auto resistor = new CircuitSim::Resistor(1, 0, 0, 100.0);
    auto source = new CircuitSim::PowerSource(2, 100, 0, 5.0);
    auto bulb = new CircuitSim::Bulb(3, 200, 0, 10.0);
    auto ground = new CircuitSim::Ground(4, 300, 0);
    
    circuit.addComponent(resistor);
    circuit.addComponent(source);
    circuit.addComponent(bulb);
    circuit.addComponent(ground);
    
    circuit.connect(resistor->getPorts()[0], source->getPorts()[1]);
    circuit.connect(resistor->getPorts()[1], bulb->getPorts()[0]);
    circuit.connect(bulb->getPorts()[1], ground->getPorts()[0]);
    circuit.connect(source->getPorts()[0], ground->getPorts()[0]);
    
    auto errors = circuit.validateCircuit();
    if (!errors.empty()) {
        std::cout << "Validation Errors:" << std::endl;
        for (const auto& err : errors) {
            std::cout << "  - " << err << std::endl;
        }
    } else {
        std::cout << "Circuit validation passed!" << std::endl;
    }
    
    if (circuit.solve()) {
        auto results = circuit.getResults();
        printResults(results);
        
        circuit.logSimulation("Basic series circuit test");
    } else {
        std::cout << "Simulation failed!" << std::endl;
    }
}

void testRCircuit() {
    std::cout << "\n=== Test 2: RC Circuit ===" << std::endl;
    
    CircuitSim::Circuit circuit;
    
    auto resistor = new CircuitSim::Resistor(1, 0, 0, 1000.0);
    auto capacitor = new CircuitSim::Capacitor(2, 100, 0, 1e-6);
    auto source = new CircuitSim::PowerSource(3, 200, 0, 10.0);
    auto ground = new CircuitSim::Ground(4, 300, 0);
    
    circuit.addComponent(resistor);
    circuit.addComponent(capacitor);
    circuit.addComponent(source);
    circuit.addComponent(ground);
    
    circuit.connect(source->getPorts()[0], resistor->getPorts()[0]);
    circuit.connect(resistor->getPorts()[1], capacitor->getPorts()[0]);
    circuit.connect(capacitor->getPorts()[1], ground->getPorts()[0]);
    circuit.connect(source->getPorts()[1], ground->getPorts()[0]);
    
    if (circuit.solve()) {
        auto results = circuit.getResults();
        printResults(results);
        
        circuit.logSimulation("RC circuit test");
    } else {
        std::cout << "Simulation failed!" << std::endl;
    }
}

void testRLCircuit() {
    std::cout << "\n=== Test 3: RL Circuit ===" << std::endl;
    
    CircuitSim::Circuit circuit;
    
    auto resistor = new CircuitSim::Resistor(1, 0, 0, 10.0);
    auto inductor = new CircuitSim::Inductor(2, 100, 0, 0.01);
    auto source = new CircuitSim::PowerSource(3, 200, 0, 12.0);
    auto ground = new CircuitSim::Ground(4, 300, 0);
    
    circuit.addComponent(resistor);
    circuit.addComponent(inductor);
    circuit.addComponent(source);
    circuit.addComponent(ground);
    
    circuit.connect(source->getPorts()[0], resistor->getPorts()[0]);
    circuit.connect(resistor->getPorts()[1], inductor->getPorts()[0]);
    circuit.connect(inductor->getPorts()[1], ground->getPorts()[0]);
    circuit.connect(source->getPorts()[1], ground->getPorts()[0]);
    
    if (circuit.solve()) {
        auto results = circuit.getResults();
        printResults(results);
        
        circuit.logSimulation("RL circuit test");
    } else {
        std::cout << "Simulation failed!" << std::endl;
    }
}

void testSwitchCircuit() {
    std::cout << "\n=== Test 4: Switch Circuit ===" << std::endl;
    
    CircuitSim::Circuit circuit;
    
    auto resistor = new CircuitSim::Resistor(1, 0, 0, 100.0);
    auto bulb = new CircuitSim::Bulb(2, 100, 0, 5.0);
    auto sw = new CircuitSim::Switch(3, 200, 0, false);
    auto source = new CircuitSim::PowerSource(4, 300, 0, 5.0);
    auto ground = new CircuitSim::Ground(5, 400, 0);
    
    circuit.addComponent(resistor);
    circuit.addComponent(bulb);
    circuit.addComponent(sw);
    circuit.addComponent(source);
    circuit.addComponent(ground);
    
    circuit.connect(source->getPorts()[0], resistor->getPorts()[0]);
    circuit.connect(resistor->getPorts()[1], bulb->getPorts()[0]);
    circuit.connect(bulb->getPorts()[1], sw->getPorts()[0]);
    circuit.connect(sw->getPorts()[1], ground->getPorts()[0]);
    circuit.connect(source->getPorts()[1], ground->getPorts()[0]);
    
    std::cout << "Switch OPEN:" << std::endl;
    if (circuit.solve()) {
        auto results = circuit.getResults();
        printResults(results);
    }
    
    std::cout << "\nClosing switch..." << std::endl;
    sw->setState(true);
    
    if (circuit.solve()) {
        auto results = circuit.getResults();
        printResults(results);
        
        circuit.logSimulation("Switch closed test");
    }
}

void testCircuitValidator() {
    std::cout << "\n=== Test 5: Circuit Validator ===" << std::endl;
    
    CircuitSim::Circuit circuit;
    
    auto resistor = new CircuitSim::Resistor(1, 0, 0, 100.0);
    auto bulb = new CircuitSim::Bulb(2, 100, 0, 5.0);
    
    circuit.addComponent(resistor);
    circuit.addComponent(bulb);
    
    auto errors = circuit.validateCircuit();
    std::cout << "Validation without ground and source:" << std::endl;
    for (const auto& err : errors) {
        std::cout << "  - " << err << std::endl;
    }
    
    auto source = new CircuitSim::PowerSource(3, 200, 0, 5.0);
    auto ground = new CircuitSim::Ground(4, 300, 0);
    circuit.addComponent(source);
    circuit.addComponent(ground);
    
    circuit.connect(source->getPorts()[0], resistor->getPorts()[0]);
    circuit.connect(resistor->getPorts()[1], bulb->getPorts()[0]);
    circuit.connect(bulb->getPorts()[1], ground->getPorts()[0]);
    circuit.connect(source->getPorts()[1], ground->getPorts()[0]);
    
    errors = circuit.validateCircuit();
    std::cout << "\nAfter adding ground and connecting:" << std::endl;
    if (errors.empty()) {
        std::cout << "  All validations passed!" << std::endl;
    } else {
        for (const auto& err : errors) {
            std::cout << "  - " << err << std::endl;
        }
    }
}

void testSimulationLogger() {
    std::cout << "\n=== Test 6: Simulation Logger ===" << std::endl;
    
    auto& logger = CircuitSim::SimulationLogger::getInstance();
    
    std::cout << "Number of logged simulations: " << logger.getLogCount() << std::endl;
    
    if (logger.getLogCount() > 0) {
        std::cout << "\nExporting log to CSV..." << std::endl;
        if (logger.exportCSV("simulation_log.csv")) {
            std::cout << "Export successful!" << std::endl;
        } else {
            std::cout << "Export failed!" << std::endl;
        }
    }
}

int main() {
    std::cout << "=== Circuit Simulation System - Enhanced Version ===" << std::endl;
    
    testBasicCircuit();
    testRCircuit();
    testRLCircuit();
    testSwitchCircuit();
    testCircuitValidator();
    testSimulationLogger();
    
    std::cout << "\n=== All tests completed ===" << std::endl;
    
    return 0;
}