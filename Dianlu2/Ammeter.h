#ifndef AMMETER_H
#define AMMETER_H
#include "Component.h"

class Ammeter : public Component {
public:
    Ammeter(int comp_id);
    ~Ammeter() override {}
    void UpdateState() override;
};
#endif