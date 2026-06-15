#include "Branch.h"
#include "Component.h"
#include "Node.h"

namespace CircuitSim {

Branch::Branch(int id, Component* component, Node* nodeA, Node* nodeB)
    : id_(id), component_(component), nodeA_(nodeA), nodeB_(nodeB), 
      current_(0.0), voltage_(0.0) {}

bool Branch::isConnected() const {
    return nodeA_ != nullptr && nodeB_ != nullptr;
}

double Branch::getVoltage() const {
    if (!isConnected()) return 0.0;
    voltage_ = nodeA_->getVoltage() - nodeB_->getVoltage();
    return voltage_;
}

std::string Branch::getCurrentDirection() const {
    if (current_ > 1e-10) {
        return "A -> B";
    } else if (current_ < -1e-10) {
        return "B -> A";
    }
    return "No Flow";
}

double Branch::getPower() const {
    return voltage_ * current_;
}

std::string Branch::getPowerFlow() const {
    double power = getPower();
    if (power > 1e-10) {
        return "Absorbing";
    } else if (power < -1e-10) {
        return "Supplying";
    }
    return "Neutral";
}

bool Branch::isPowerAbsorbing() const {
    return getPower() > 1e-10;
}

bool Branch::isPowerSupplying() const {
    return getPower() < -1e-10;
}

} // namespace CircuitSim