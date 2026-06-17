#include "Resistor.h"

// 构造函数实现
// 注意后面的 ": Component(comp_id)"，这叫【初始化列表】
// 意思是：在造电阻之前，先把图纸里的基础属性(id)交给父类去初始化
Resistor::Resistor(int comp_id, double default_resistance) : Component(comp_id) {
    // 把外界传进来的阻值，赋给从父类继承来的 resistance 变量
    resistance = default_resistance;
    
    // 注意：voltage 和 current 已经在父类的构造函数里自动被设为 0 了，这里不用再写
}

// 🔥 实现电阻独有的状态更新逻辑（物理定律）
void Resistor::UpdateState() {
    // 严谨的高分程序员习惯：做除法前，必须判断分母不为0，防止程序崩溃(抛出异常)
    if (resistance > 0) {
        // 欧姆定律：电流 = 电压 / 电阻
        // 假设电路求解器算出了这个电阻两端的 voltage，这里就能瞬间更新自己的 current
        current = voltage / resistance;
    } else {
        current = 0.0;
    }
}