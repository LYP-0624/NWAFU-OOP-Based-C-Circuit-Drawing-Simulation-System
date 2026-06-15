#include <iostream>
#include <iomanip>
#include "Circuit.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Bulb.h"
#include "Switch.h"
#include "Ammeter.h"
#include "Voltmeter.h"

using namespace CircuitSim;

void printSeparator() {
    std::cout << std::string(80, '=') << std::endl;
}

void printResults(const std::vector<SimulationResult>& results) {
    std::cout << std::left << std::setw(12) << "ID"
              << std::setw(15) << "Name"
              << std::setw(15) << "Type"
              << std::setw(12) << "Voltage(V)"
              << std::setw(12) << "Current(A)"
              << std::setw(12) << "Power(W)"
              << std::setw(8) << "Active"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& res : results) {
        std::cout << std::left << std::setw(12) << res.componentId
                  << std::setw(15) << res.componentName
                  << std::setw(15) << res.componentType
                  << std::setw(12) << std::fixed << std::setprecision(4) << res.voltage
                  << std::setw(12) << std::fixed << std::setprecision(4) << res.current
                  << std::setw(12) << std::fixed << std::setprecision(4) << res.power
                  << std::setw(8) << (res.isActive ? "Yes" : "No")
                  << std::endl;
    }
}

void testSimpleSeriesCircuit() {
    printSeparator();
    std::cout << "Test 1: Simple Series Circuit (5V + 100ohm + 10ohm Bulb + Closed Switch)" << std::endl;
    printSeparator();
    
    Circuit circuit;
    
    PowerSource* ps = new PowerSource(1, 0, 0, 5.0);
    Resistor* r1 = new Resistor(2, 100, 0, 100.0);
    Bulb* bulb = new Bulb(3, 200, 0, 10.0, 0.5);
    Switch* sw = new Switch(4, 300, 0, true);
    
    circuit.addComponent(ps);
    circuit.addComponent(r1);
    circuit.addComponent(bulb);
    circuit.addComponent(sw);
    
    auto& psPorts = ps->getPorts();
    auto& r1Ports = r1->getPorts();
    auto& bulbPorts = bulb->getPorts();
    auto& swPorts = sw->getPorts();
    
    circuit.connect(psPorts[0], r1Ports[0]);
    circuit.connect(r1Ports[1], bulbPorts[0]);
    circuit.connect(bulbPorts[1], swPorts[0]);
    circuit.connect(swPorts[1], psPorts[1]);
    
    std::cout << "\nSolving circuit..." << std::endl;
    if (circuit.solve()) {
        std::cout << "Solve successful!" << std::endl;
        std::cout << "\n=== Simulation Results ===" << std::endl;
        printResults(circuit.getResults());
        
        double totalR = 100.0 + 10.0 + 0.001;
        double expectedI = 5.0 / totalR;
        std::cout << "\nExpected current: " << std::fixed << std::setprecision(4) << expectedI << "A" << std::endl;
    } else {
        std::cout << "Solve failed!" << std::endl;
    }
}

void testCircuitWithAmmeter() {
    printSeparator();
    std::cout << "Test 2: Series Circuit with Ammeter" << std::endl;
    printSeparator();
    
    Circuit circuit;
    
    PowerSource* ps = new PowerSource(1, 0, 0, 12.0);
    Resistor* r1 = new Resistor(2, 100, 0, 100.0);
    Ammeter* ammeter = new Ammeter(3, 200, 0);
    
    circuit.addComponent(ps);
    circuit.addComponent(r1);
    circuit.addComponent(ammeter);
    
    circuit.connect(ps->getPorts()[0], r1->getPorts()[0]);
    circuit.connect(r1->getPorts()[1], ammeter->getPorts()[0]);
    circuit.connect(ammeter->getPorts()[1], ps->getPorts()[1]);
    
    std::cout << "\nSolving circuit..." << std::endl;
    if (circuit.solve()) {
        std::cout << "Solve successful!" << std::endl;
        std::cout << "\n=== Simulation Results ===" << std::endl;
        printResults(circuit.getResults());
        
        std::cout << "\nAmmeter reading should be: " << std::fixed << std::setprecision(4) 
                  << (12.0 / 100.0) << "A" << std::endl;
    } else {
        std::cout << "Solve failed!" << std::endl;
    }
}

void testOpenSwitchCircuit() {
    printSeparator();
    std::cout << "Test 3: Open Switch Circuit" << std::endl;
    printSeparator();
    
    Circuit circuit;
    
    PowerSource* ps = new PowerSource(1, 0, 0, 5.0);
    Resistor* r1 = new Resistor(2, 100, 0, 100.0);
    Switch* sw = new Switch(3, 200, 0, false);
    
    circuit.addComponent(ps);
    circuit.addComponent(r1);
    circuit.addComponent(sw);
    
    circuit.connect(ps->getPorts()[0], r1->getPorts()[0]);
    circuit.connect(r1->getPorts()[1], sw->getPorts()[0]);
    circuit.connect(sw->getPorts()[1], ps->getPorts()[1]);
    
    std::cout << "\nSolving circuit (switch open)..." << std::endl;
    if (circuit.solve()) {
        std::cout << "Solve successful!" << std::endl;
        std::cout << "\n=== Simulation Results ===" << std::endl;
        printResults(circuit.getResults());
    } else {
        std::cout << "Solve failed!" << std::endl;
    }
}

void testJSONExport() {
    printSeparator();
    std::cout << "Test 4: JSON Export" << std::endl;
    printSeparator();
    
    Circuit circuit;
    
    PowerSource* ps = new PowerSource(1, 0, 0, 5.0);
    Resistor* r1 = new Resistor(2, 100, 0, 100.0);
    
    circuit.addComponent(ps);
    circuit.addComponent(r1);
    
    circuit.connect(ps->getPorts()[0], r1->getPorts()[0]);
    circuit.connect(r1->getPorts()[1], ps->getPorts()[1]);
    
    std::cout << "\n=== JSON Export ===" << std::endl;
    std::cout << circuit.toJson() << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Circuit Simulation System - Physics Layer Test" << std::endl;
    std::cout << "========================================" << std::endl;
    
    testSimpleSeriesCircuit();
    testCircuitWithAmmeter();
    testOpenSwitchCircuit();
    testJSONExport();
    
    printSeparator();
    std::cout << "All tests completed!" << std::endl;
    printSeparator();
    
    return 0;
}