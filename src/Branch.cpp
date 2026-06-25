#include "Branch.h"

#include "Component.h"
#include "Node.h"

namespace CircuitSim {

// 构造函数：基于元器件和两端节点创建支路（用于仿真结果）
Branch::Branch(int id, Component* component, Node* nodeA, Node* nodeB)
    : id_(id),
      component_(component),      // 关联的元器件
      startComponent_(nullptr),   // 起始元器件（连线模式用）
      endComponent_(nullptr),     // 终止元器件（连线模式用）
      nodeA_(nodeA),              // 节点A
      nodeB_(nodeB),              // 节点B
      startPortIdx_(0),           // 起始端口索引
      endPortIdx_(1),             // 终止端口索引
      current_(0.0),              // 支路电流
      voltage_(0.0),              // 支路电压
      resistance_(0.0) {}         // 支路电阻

// 构造函数：基于两端元器件和端口索引创建支路（用于图形化连线）
Branch::Branch(int id, Component* startComp, int startPortIdx, Component* endComp, int endPortIdx)
    : id_(id),
      component_(nullptr),        // 无直接关联元器件
      startComponent_(startComp), // 起始元器件
      endComponent_(endComp),     // 终止元器件
      nodeA_(nullptr),            // 节点A（待连接）
      nodeB_(nullptr),            // 节点B（待连接）
      startPortIdx_(startPortIdx),// 起始端口索引
      endPortIdx_(endPortIdx),    // 终止端口索引
      current_(0.0),
      voltage_(0.0),
      resistance_(0.0) {}

// 判断支路两端是否都已连接到节点
bool Branch::isConnected() const {
    return nodeA_ != nullptr && nodeB_ != nullptr;
}

// 获取支路两端电压差（节点A电压减节点B电压）
double Branch::getVoltage() const {
    if (!isConnected()) {
        return 0.0;
    }
    return nodeA_->getVoltage() - nodeB_->getVoltage();
}

// 获取电流方向描述
std::string Branch::getCurrentDirection() const {
    if (current_ > 1e-10) {       // 正向电流（A到B）
        return "A -> B";
    }
    if (current_ < -1e-10) {      // 反向电流（B到A）
        return "B -> A";
    }
    return "No Flow";             // 无电流
}

// 计算支路功率（电压×电流）
double Branch::getPower() const {
    return voltage_ * current_;
}

// 获取功率流向描述（吸收/提供/中性）
std::string Branch::getPowerFlow() const {
    double power = getPower();
    if (power > 1e-10) {          // 吸收功率
        return "Absorbing";
    }
    if (power < -1e-10) {         // 提供功率
        return "Supplying";
    }
    return "Neutral";             // 功率近似为零
}

// 判断是否吸收功率
bool Branch::isPowerAbsorbing() const {
    return getPower() > 1e-10;
}

// 判断是否提供功率
bool Branch::isPowerSupplying() const {
    return getPower() < -1e-10;
}

} // namespace CircuitSim