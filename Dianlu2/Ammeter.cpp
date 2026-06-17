#include "Ammeter.h"

Ammeter::Ammeter(int comp_id) : Component(comp_id) {
    resistance = 0.0; // 理想电流表内阻为 0
}

void Ammeter::UpdateState() {
    // 理想电流表两端没有电压降
    voltage = 0.0;
}