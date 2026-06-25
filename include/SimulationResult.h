#ifndef SIMULATION_RESULT_H
#define SIMULATION_RESULT_H

#include <map>
#include <string>

namespace CircuitSim {

// 仿真结果结构体，存储单个元器件的仿真数据
struct SimulationResult {
    int componentId;                    // 元器件唯一编号
    std::string componentName;          // 元器件名称
    std::string componentType;          // 元器件类型（如Resistor、VoltageSource等）
    double voltage;                     // 元器件两端电压（单位：伏特V）
    double current;                     // 流过元器件的电流（单位：安培A）
    double power;                       // 元器件消耗/产生的功率（单位：瓦特W）
    bool isActive;                      // 元器件是否处于工作状态
    std::map<std::string, double> extra; // 额外属性（如电阻值、电容值等扩展数据）
};

} // namespace CircuitSim

#endif