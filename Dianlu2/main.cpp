#include <iostream>
#include "CircuitSolver.h"
#include "PowerSource.h"
#include "Resistor.h"
#include "LightBulb.h"

using namespace std;

int main() {
    cout << "=== Circuit Solver Test Start ===" << endl;

    // 1. Create circuit solver
    CircuitSolver solver;

    // 2. Create components:
    //    Component 1: 9V battery
    //    Component 2: 10 ohm resistor
    //    Component 3: 10 ohm light bulb
    PowerSource* battery = new PowerSource(1, 9.0);
    Resistor* r1 = new Resistor(2, 10.0);
    LightBulb* bulb = new LightBulb(3, 10.0);

    // Set node connections for series circuit: Battery+ -> R1 -> Bulb -> Ground
    battery->SetNodes(1, 0);  // Battery: node1(+) to node0(ground/-)
    r1->SetNodes(1, 2);       // Resistor: node1 to node2
    bulb->SetNodes(2, 0);     // Bulb: node2 to node0(ground)

    // 3. Add components to solver
    solver.AddComponent(battery);
    solver.AddComponent(r1);
    solver.AddComponent(bulb);

    // 4. Check state before switch on
    cout << "\n[Switch OFF]" << endl;
    cout << "Light bulb state: " << (bulb->GetIsLit() ? "ON" : "OFF") << endl;

    // 5. Turn on switch - this triggers CalculateCircuit()
    cout << "\n[Switch ON]" << endl;
    solver.SetSwitch(true);

    // 6. Print results
    cout << "Battery voltage: " << battery->GetVoltage() << " V" << endl;
    cout << "Total current: " << bulb->GetCurrent() << " A" << endl; 
    cout << "Light bulb state: " << (bulb->GetIsLit() ? "ON" : "OFF") << endl;

    cout << "\n=== Test Complete ===" << endl;
    
    return 0;
}
