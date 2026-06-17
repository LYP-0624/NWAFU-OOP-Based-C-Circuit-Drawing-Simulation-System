#ifndef INDUCTOR_H
#define INDUCTOR_H
#include "Component.h"

class Inductor : public Component {
private:
    double inductance; // 电感值，单位亨利(H)
public:
    Inductor(int comp_id, double l_value);
    ~Inductor() override {}
    void UpdateState() override;
};
#endif