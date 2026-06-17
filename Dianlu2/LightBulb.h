#ifndef LIGHTBULB_H
#define LIGHTBULB_H

#include "Resistor.h" // 包含父类电阻的头文件

// 小灯泡类，公有继承自 Resistor (因为灯泡本质上是有阻值的发热元件)
class LightBulb : public Resistor {
private:
    // 小灯泡独有的“基因”：亮还是不亮
    bool isLit; 

public:
    // 构造函数
    LightBulb(int comp_id, double bulb_resistance);

    // 析构函数
    ~LightBulb() override {}

    // 重写状态更新函数：不仅要算欧姆定律，还得判断发不发光
    void UpdateState() override; 

    // 对外接口：让同学B（前端）用来判断要在屏幕上画黄色的亮灯泡，还是灰色的灭灯泡
    bool GetIsLit() const { return isLit; }
};

#endif // LIGHTBULB_H