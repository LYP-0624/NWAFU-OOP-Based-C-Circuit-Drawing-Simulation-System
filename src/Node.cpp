#include "Node.h"

#include <algorithm>

#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化节点ID、电压和接地状态
Node::Node(int id) : id_(id), voltage_(0.0), isGround_(false) {}

// 析构函数：断开所有连接到此节点的端口
Node::~Node() {
    // 复制端口列表，避免在遍历过程中修改原列表导致迭代器失效
    auto portsCopy = ports_;
    for (auto* p : portsCopy) {
        if (p) {
            p->disconnect();  // 断开端口与节点的连接
        }
    }
}

// 添加端口到节点（避免重复添加）
void Node::addPort(Port* port) {
    if (port && std::find(ports_.begin(), ports_.end(), port) == ports_.end()) {
        ports_.push_back(port);
    }
}

// 从节点移除指定端口
void Node::removePort(Port* port) {
    ports_.erase(std::remove(ports_.begin(), ports_.end(), port), ports_.end());
}

// 获取连接到此节点的元器件数量（通过端口数统计）
size_t Node::getComponentCount() const {
    return ports_.size();
}

} // namespace CircuitSim