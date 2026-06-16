#include "Branch.h"
#include "Component.h"
#include "Node.h"

namespace CircuitSim {

Branch::Branch(int id, Component* component, Node* nodeA, Node* nodeB)
    : id_(id), component_(component), startComponent_(nullptr), endComponent_(nullptr),
      nodeA_(nodeA), nodeB_(nodeB), startPortIdx_(0), endPortIdx_(0), 
      current_(0.0), voltage_(0.0), resistance_(0.0) {}

Branch::Branch(int id, Component* startComp, int startPortIdx, Component* endComp, int endPortIdx)
    : id_(id), component_(nullptr), startComponent_(startComp), endComponent_(endComp),
      nodeA_(nullptr), nodeB_(nullptr), startPortIdx_(startPortIdx), endPortIdx_(endPortIdx),
      current_(0.0), voltage_(0.0), resistance_(0.0) {}

bool Branch::isConnected() const {
    return nodeA_ != nullptr && nodeB_ != nullptr;
}

double Branch::getVoltage() const {
    if (!isConnected()) return 0.0;
    return nodeA_->getVoltage() - nodeB_->getVoltage();
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