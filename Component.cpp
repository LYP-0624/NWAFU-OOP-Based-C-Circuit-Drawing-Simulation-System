#include "Component.h"
#include "Port.h"

namespace CircuitSim {

Component::Component(int id, const std::string& name, double x, double y)
    : id_(id), name_(name), x_(x), y_(y), rotation_(0), selected_(false) {}

Component::~Component() {
    for (auto* p : ports_) {
        if (p) {
            p->disconnect();
            delete p;
        }
    }
}

void Component::addPort(Port* port) {
    if (port) {
        ports_.push_back(port);
    }
}

std::string Component::toString() const {
    return "Component[" + std::to_string(id_) + "] " + name_;
}

} // namespace CircuitSim