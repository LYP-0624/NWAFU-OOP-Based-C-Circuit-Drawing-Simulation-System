#include "Branch.h"
#include "Component.h"
#include "Node.h"

namespace CircuitSim {

Branch::Branch(int id, Component* component, Node* nodeA, Node* nodeB)
    : id_(id), component_(component), nodeA_(nodeA), nodeB_(nodeB), current_(0.0) {}

bool Branch::isConnected() const {
    return nodeA_ != nullptr && nodeB_ != nullptr;
}

double Branch::getVoltage() const {
    if (!isConnected()) return 0.0;
    return nodeA_->getVoltage() - nodeB_->getVoltage();
}

} // namespace CircuitSim