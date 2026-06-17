#ifndef RESISTOR_H
#define RESISTOR_H

#include "Component.h" // 必须包含基类的头文件，因为我们要继承它

// 电阻类，公有继承自 Component
class Resistor : public Component {
public:
    // 构造函数：除了需要分配身份证号(id)，还需要给它一个初始的电阻值
    Resistor(int comp_id, double default_resistance);

    // 析构函数：加上 override 关键字，表示重写
    ~Resistor() override {}

    // 🔥 核心：还记得基类里逼着我们交的“作业”吗？
    // 这里我们必须重写 UpdateState() 函数，否则 Resistor 依然会报错
    void UpdateState() override; 
};

#endif // RESISTOR_H