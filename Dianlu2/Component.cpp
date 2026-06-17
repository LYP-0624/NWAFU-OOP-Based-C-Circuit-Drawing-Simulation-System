#include "Component.h"

// 构造函数实现：接收 ID 并赋值，所有物理量默认归零
Component::Component(int comp_id) {
    id = comp_id;
    voltage = 0.0;
    current = 0.0;
    resistance = 0.0;
    node1 = -1; 
    node2 = -1;
}