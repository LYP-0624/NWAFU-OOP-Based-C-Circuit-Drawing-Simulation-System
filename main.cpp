#include <iostream>
#include "Circuit.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Bulb.h"
#include "Ammeter.h"
#include "Voltmeter.h"
#include "Switch.h"

using namespace CircuitSim;

int main() {
    std::cout << "=== 简易电路图生成系统 测试 ===" << std::endl;
    
    Circuit circuit;
    
    // 创建元器件
    PowerSource* ps = new PowerSource(1, 0, 0, 5.0);
    Resistor* r1 = new Resistor(2, 100, 0, 100.0);
    Bulb* bulb = new Bulb(3, 200, 0, 10.0, 0.5);
    Ammeter* am = new Ammeter(4, 50, 50);
    Voltmeter* vm = new Voltmeter(5, 150, 50);
    Switch* sw = new Switch(6, 250, 0, true);
    
    circuit.addComponent(ps);
    circuit.addComponent(r1);
    circuit.addComponent(bulb);
    circuit.addComponent(am);
    circuit.addComponent(vm);
    circuit.addComponent(sw);
    
    // 连接：电源+ -> 电阻 -> 灯泡 -> 开关 -> 电源-
    // 简化测试：直接连接端口
    auto& psPorts = ps->getPorts();
    auto& r1Ports = r1->getPorts();
    auto& bulbPorts = bulb->getPorts();
    
    // 电源正极接电阻
    circuit.connect(psPorts[0], r1Ports[0]);
    // 电阻接灯泡
    circuit.connect(r1Ports[1], bulbPorts[0]);
    
    // 求解
    std::cout << "\n求解电路..." << std::endl;
    if (circuit.solve()) {
        std::cout << "求解成功！" << std::endl;
        
        auto results = circuit.getResults();
        std::cout << "\n=== 仿真结果 ===" << std::endl;
        for (const auto& res : results) {
            std::cout << "ID: " << res.componentId 
                      << " | 名称: " << res.componentName
                      << " | 类型: " << res.componentType
                      << " | 电压: " << res.voltage << "V"
                      << " | 电流: " << res.current << "A"
                      << " | 功率: " << res.power << "W"
                      << " | 激活: " << (res.isActive ? "是" : "否");
            
            if (!res.extra.empty()) {
                std::cout << " | 读数: " << res.extra.begin()->second;
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "求解失败！" << std::endl;
    }
    
    // 测试序列化
    std::cout << "\n=== JSON导出 ===" << std::endl;
    std::cout << circuit.toJson() << std::endl;
    
    return 0;
}