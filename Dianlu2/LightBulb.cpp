#include "LightBulb.h"
#include <cmath> // 引入数学库，方便用 std::abs 取绝对值

// 构造函数：把 id 和 阻值 直接丢给父类 Resistor 去处理，自己只负责把 isLit 设为 false
LightBulb::LightBulb(int comp_id, double bulb_resistance) 
    : Resistor(comp_id, bulb_resistance) {
    isLit = false; // 没通电前，默认灯是灭的
}

// 🔥 核心逻辑：灯泡通电时的反应
void LightBulb::UpdateState() {
    // 1. 偷懒绝招：直接呼叫父类（电阻）的函数，帮你把欧姆定律（电流 = 电压/电阻）算完！
    Resistor::UpdateState(); 

    // 2. 算完之后，检查刚刚算出来的 current（电流）
    // 只要流过灯泡的电流绝对值大于一个很小的阈值（比如 0.001A），就判定为发光
    if (std::abs(current) > 0.001) {
        isLit = true;  // 亮起！
    } else {
        isLit = false; // 熄灭！
    }
}