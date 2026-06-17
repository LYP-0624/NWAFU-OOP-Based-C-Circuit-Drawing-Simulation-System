#include "Capacitor.h"

Capacitor::Capacitor(int comp_id, double c_value) : Component(comp_id) {
    capacitance = c_value;
    resistance = 1e9; // 直流稳态下，电容相当于断路（无穷大电阻）
}

void Capacitor::UpdateState() {
    // 稳态下没有电流流过电容
}