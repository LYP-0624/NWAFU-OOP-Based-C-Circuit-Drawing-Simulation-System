#ifndef VOLTMETER_H
#define VOLTMETER_H
#include "Component.h"

class Voltmeter : public Component {
public:
    Voltmeter(int comp_id);
    ~Voltmeter() override {}
    void UpdateState() override;
};
#endif