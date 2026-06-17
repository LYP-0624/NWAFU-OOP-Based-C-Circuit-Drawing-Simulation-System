#ifndef SWITCH_H
#define SWITCH_H
#include "Component.h"

class Switch : public Component {
private:
    bool isClosed; // 记录自身是否闭合
public:
    Switch(int comp_id);
    ~Switch() override {}

    void Toggle(); // 拨动开关的动作
    bool GetIsClosed() const { return isClosed; }
    void UpdateState() override;
};
#endif