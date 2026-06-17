#include "Voltmeter.h"

Voltmeter::Voltmeter(int comp_id) : Component(comp_id) {
    resistance = 1e9; // 理想电压表内阻无穷大（断路）
}

void Voltmeter::UpdateState() {
    // 根据欧姆定律算出自己分到的电压（如果是并联电路，求解器会处理）
    voltage = current * resistance; 
}