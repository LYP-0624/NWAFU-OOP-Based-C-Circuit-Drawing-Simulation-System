#include "Switch.h"

Switch::Switch(int comp_id) : Component(comp_id) {
    isClosed = false;
    resistance = 1e9; // 默认断开，电阻极大（相当于断路）
}

void Switch::Toggle() {
    isClosed = !isClosed;
    // 闭合时电阻为0（理想导线），断开时为1e9（断路）
    resistance = isClosed ? 0.0 : 1e9; 
}

void Switch::UpdateState() {
    // 开关自身不消耗能量，状态更新主要由 Toggle 触发改变电阻
    // 这里的电流电压由求解器算完后直接用基类的 SetCurrent 塞进来
}