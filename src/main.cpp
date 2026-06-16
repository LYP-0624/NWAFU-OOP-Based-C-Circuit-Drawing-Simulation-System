#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
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
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "                 Simulation Results\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << std::setw(8) << "ID" 
              << std::setw(14) << "Name" 
              << std::setw(14) << "Type"
              << std::setw(12) << "Voltage(V)"
              << std::setw(12) << "Current(A)"
              << std::setw(12) << "Power(W)"
              << std::setw(10) << "Status"
              << std::endl;
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& r : results) {
        std::cout << std::setw(8) << r.componentId
                  << std::setw(14) << r.componentName
                  << std::setw(14) << r.componentType
                  << std::setw(12) << std::fixed << std::setprecision(4) << r.voltage
                  << std::setw(12) << std::fixed << std::setprecision(6) << r.current
                  << std::setw(12) << std::fixed << std::setprecision(4) << r.power
                  << std::setw(10) << (r.isActive ? "Active" : "Inactive")
                  << std::endl;
    }
    std::cout << std::string(70, '=') << "\n";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::cout << "\n" << std::string(70, '*') << "\n";
    std::cout << "      Circuit Simulation System v1.0\n";
    std::cout << std::string(70, '*') << "\n";
    
    CircuitSim::Circuit circuit;
    int nextId = 1;
    
    while (true) {
        std::cout << "\n" << std::string(70, '-') << "\n";
        std::cout << "[Main Menu]\n";
        std::cout << "1. Add Component\n";
        std::cout << "2. Connect Components\n";
        std::cout << "3. Delete Component\n";
        std::cout << "4. Run Simulation\n";
        std::cout << "5. Validate Circuit\n";
        std::cout << "6. View Circuit Info\n";
        std::cout << "7. Export Log\n";
        std::cout << "8. Exit\n";
        std::cout << std::string(70, '-') << "\n";
        std::cout << "Enter your choice (1-8): ";
        
        int choice;
        std::cin >> choice;
        clearInput();
        
        if (choice == 1) {
            std::cout << "\n[Add Component]\n";
            std::cout << "1. 电阻 (Resistor)\n";
            std::cout << "2. 电源 (PowerSource)\n";
            std::cout << "3. 灯泡 (Bulb)\n";
            std::cout << "4. 开关 (Switch)\n";
            std::cout << "5. 接地 (Ground)\n";
            std::cout << "6. 电容 (Capacitor)\n";
            std::cout << "7. 电感 (Inductor)\n";
            std::cout << "Select component type (1-7): ";
            
            int type;
            std::cin >> type;
            clearInput();
            
            CircuitSim::Component* comp = nullptr;
            switch (type) {
                case 1: {
                    double r;
                    std::cout << "Enter resistance (Ohm): ";
                    std::cin >> r;
                    comp = new CircuitSim::Resistor(nextId++, 0, 0, r);
                    break;
                }
                case 2: {
                    double v;
                    std::cout << "Enter voltage (Volt): ";
                    std::cin >> v;
                    comp = new CircuitSim::PowerSource(nextId++, 0, 0, v);
                    break;
                }
                case 3: {
                    double r;
                    std::cout << "Enter bulb resistance (Ohm): ";
                    std::cin >> r;
                    comp = new CircuitSim::Bulb(nextId++, 0, 0, r);
                    break;
                }
                case 4:
                    comp = new CircuitSim::Switch(nextId++, 0, 0, false);
                    break;
                case 5:
                    comp = new CircuitSim::Ground(nextId++, 0, 0);
                    break;
                case 6: {
                    double c;
                    std::cout << "Enter capacitance (Farad, e.g. 1e-6): ";
                    std::cin >> c;
                    comp = new CircuitSim::Capacitor(nextId++, 0, 0, c);
                    break;
                }
                case 7: {
                    double l;
                    std::cout << "Enter inductance (Henry, e.g. 0.01): ";
                    std::cin >> l;
                    comp = new CircuitSim::Inductor(nextId++, 0, 0, l);
                    break;
                }
                default:
                    std::cout << "Invalid choice!\n";
                    continue;
            }
            
            if (comp) {
                circuit.addComponent(comp);
                std::cout << "Component added successfully! ID: " << comp->getId() << "\n";
            }
        }
        
        else if (choice == 2) {
            std::cout << "\n[Connect Components]\n";
            int id1, port1, id2, port2;
            std::cout << "Enter first component ID: ";
            std::cin >> id1;
            std::cout << "Enter first component port (0 or 1): ";
            std::cin >> port1;
            std::cout << "Enter second component ID: ";
            std::cin >> id2;
            std::cout << "输入第二个元件端口 (0或1): ";
            std::cin >> port2;
            clearInput();
            
            auto c1 = circuit.getComponent(id1);
            auto c2 = circuit.getComponent(id2);
            
            if (c1 && c2) {
                auto& ports1 = c1->getPorts();
                auto& ports2 = c2->getPorts();
                if (port1 >= 0 && port1 < (int)ports1.size() && 
                    port2 >= 0 && port2 < (int)ports2.size()) {
                    circuit.connect(ports1[port1], ports2[port2]);
                    std::cout << "Connection successful!\n";
                } else {
                    std::cout << "Invalid port number!\n";
                }
            } else {
                std::cout << "元件不存在！\n";
            }
        }
        
        else if (choice == 3) {
            std::cout << "\n[Delete Component]\n";
            int id;
            std::cout << "Enter component ID to delete: ";
            std::cin >> id;
            clearInput();
            
            circuit.removeComponent(id);
            std::cout << "元件已删除（如果存在）\n";
        }
        
        else if (choice == 4) {
            std::cout << "\n[Run Simulation]\n";
            if (circuit.solve()) {
                auto results = circuit.getResults();
                printResults(results);
                circuit.logSimulation("Manual simulation");
            } else {
                std::cout << "仿真失败！请检查电路连接\n";
            }
        }
        
        else if (choice == 5) {
            std::cout << "\n[Validate Circuit]\n";
            auto errors = circuit.validateCircuit();
            if (errors.empty()) {
                std::cout << "✓ 电路验证通过！\n";
            } else {
                std::cout << "✗ 电路验证失败:\n";
                for (const auto& err : errors) {
                    std::cout << "  - " << err << "\n";
                }
            }
        }
        
        else if (choice == 6) {
            std::cout << "\n【电路信息】\n";
            std::cout << "Component count: " << circuit.getComponentCount() << "\n";
            std::cout << "节点数量: " << circuit.getNodeCount() << "\n";
            std::cout << "Branch count: " << circuit.getBranchList().size() << "\n";
            
            auto results = circuit.getResults();
            if (!results.empty()) {
                std::cout << "\nCurrent components:\n";
                for (const auto& r : results) {
                    std::cout << "  ID:" << r.componentId 
                              << "  " << r.componentName 
                              << " (" << r.componentType << ")\n";
                }
            }
        }
        
        else if (choice == 7) {
            std::cout << "\n[Export Log]\n";
            if (CircuitSim::SimulationLogger::getInstance().exportCSV("simulation_log.csv")) {
                std::cout << "Log exported to simulation_log.csv\n";
            } else {
                std::cout << "Export failed!\n";
            }
        }
        
        else if (choice == 8) {
            std::cout << "\nThank you! Goodbye!\n";
            break;
        }
        
        else {
            std::cout << "\n无效选择，请重新输入！\n";
        }
    }
    
    return 0;
}