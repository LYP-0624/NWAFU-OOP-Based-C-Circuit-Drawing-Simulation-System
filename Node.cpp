#include "Node.h"
#include "Port.h"

namespace CircuitSim {

Node::Node(int id) : id_(id), voltage_(0.0), isGround_(false) {}

Node::~Node() {
    for (auto* p : ports_) {
        if (p) {
            p->disconnect();
        }
    }
}

void Node::addPort(Port* port) {
    if (port) {
        ports_.push_back(port);
    }
}

void Node::removePort(Port* port) {
    for (auto it = ports_.begin(); it != ports_.end(); ++it) {
        if (*it == port) {
            ports_.erase(it);
            break;
        }
    }
}

void Node::merge(Node* other) {
    if (!other || other == this) return;
    for (auto* p : other->ports_) {
        if (p) {
            p->connectTo(this);
        }
    }
    other->ports_.clear();
}

size_t Node::getComponentCount() const {
    return ports_.size();
}

} // namespace CircuitSim