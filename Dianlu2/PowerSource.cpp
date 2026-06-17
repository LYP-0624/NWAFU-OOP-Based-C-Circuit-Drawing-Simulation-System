#include "PowerSource.h"

// 构造函数实现
PowerSource::PowerSource(int comp_id, double emf) : Component(comp_id) {
    electromotiveForce = emf; // 记下这节电池的标称电压
    voltage = emf;            // 电池一出生，两端电压就是它的电动势
    resistance = 0.0;         // 在我们这个简易系统里，假设它是“理想电池”，内阻为0
}

// 🔥 核心逻辑：电池通电时的反应
void PowerSource::UpdateState() {
    voltage = electromotiveForce; 

}