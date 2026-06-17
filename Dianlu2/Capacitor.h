#ifndef CAPACITOR_H
#define CAPACITOR_H
#include "Component.h"

class Capacitor : public Component {
private:
    double capacitance; // 电容值，单位法拉(F)
public:
    Capacitor(int comp_id, double c_value);
    ~Capacitor() override {}
    void UpdateState() override;
};
#endif