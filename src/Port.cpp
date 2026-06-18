#include "Port.h"

#include "Node.h"

namespace CircuitSim {

Port::Port(Component* parent, int index)
    : parent_(parent), index_(index), node_(nullptr), connected_(false) {}

Port::~Port() {
    disconnect();
}

void Port::connectTo(Node* node) {
    if (node_ && node_ != node) {
        node_->removePort(this);
    }

    node_ = node;
    if (node_) {
        node_->addPort(this);
        connected_ = true;
    }
}

void Port::disconnect() {
    if (node_) {
        node_->removePort(this);
        node_ = nullptr;
    }
    connected_ = false;
}

double Port::getVoltage() const {
    return node_ ? node_->getVoltage() : 0.0;
}

} // namespace CircuitSim
