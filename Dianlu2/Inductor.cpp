#include "Inductor.h"

Inductor::Inductor(int comp_id, double l_value) : Component(comp_id) {
    inductance = l_value;
    resistance = 0.0; // 直流稳态下，电感相当于一根纯导线（0内阻）
}

void Inductor::UpdateState() {
    // 稳态下，电感两端没有电压降
    voltage = 0.0;
}