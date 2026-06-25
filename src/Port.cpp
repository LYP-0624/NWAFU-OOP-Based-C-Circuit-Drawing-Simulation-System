#include "Port.h"

#include "Node.h"

namespace CircuitSim {

// 构造函数：初始化端口所属元器件、索引、节点和连接状态
Port::Port(Component* parent, int index)
    : parent_(parent), index_(index), node_(nullptr), connected_(false) {}

// 析构函数：断开端口连接
Port::~Port() {
    disconnect();
}

// 将端口连接到指定节点，若原节点不同则先断开原连接
void Port::connectTo(Node* node) {
    if (node_ && node_ != node) {
        node_->removePort(this);  // 从原节点移除
    }

    node_ = node;
    if (node_) {
        node_->addPort(this);     // 添加到新节点
        connected_ = true;        // 标记为已连接
    }
}

// 断开端口与节点的连接
void Port::disconnect() {
    if (node_) {
        node_->removePort(this);  // 从节点移除
        node_ = nullptr;
    }
    connected_ = false;           // 标记为未连接
}

// 获取端口所在节点的电压，未连接时返回0
double Port::getVoltage() const {
    return node_ ? node_->getVoltage() : 0.0;
}

} // namespace CircuitSim