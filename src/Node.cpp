#include "Node.h"

#include <algorithm>

#include "Port.h"

namespace CircuitSim {

Node::Node(int id) : id_(id), voltage_(0.0), isGround_(false) {}

Node::~Node() {
    auto portsCopy = ports_;
    for (auto* p : portsCopy) {
        if (p) {
            p->disconnect();
        }
    }
}

void Node::addPort(Port* port) {
    if (port && std::find(ports_.begin(), ports_.end(), port) == ports_.end()) {
        ports_.push_back(port);
    }
}

void Node::removePort(Port* port) {
    ports_.erase(std::remove(ports_.begin(), ports_.end(), port), ports_.end());
}

size_t Node::getComponentCount() const {
    return ports_.size();
}

} // namespace CircuitSim
