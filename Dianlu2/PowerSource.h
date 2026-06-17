#ifndef POWERSOURCE_H
#define POWERSOURCE_H

#include "Component.h"

// 电源类（电池），公有继承自 Component
class PowerSource : public Component {
private:
    double electromotiveForce; // 电动势（电池标称电压，比如9V、12V）

public:
    // 构造函数：需要身份证号，以及这节电池的电动势
    PowerSource(int comp_id, double emf);

    ~PowerSource() override {}

    // 重写状态更新函数：电池的逻辑和电阻完全不一样！
    void UpdateState() override; 
};

#endif // POWERSOURCE_H