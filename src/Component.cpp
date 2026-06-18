#include "Component.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Component::Component(int id, const std::string& name, double x, double y)
    : id_(id), name_(name), x_(x), y_(y), rotation_(0.0), selected_(false) {}

Component::~Component() = default;

std::string Component::getTypeName() const {
    return ComponentFactory::typeToString(getType());
}

std::vector<Port*> Component::getPorts() const {
    std::vector<Port*> result;
    result.reserve(ports_.size());
    for (const auto& port : ports_) {
        result.push_back(port.get());
    }
    return result;
}

Port* Component::getPort(size_t index) const {
    if (index >= ports_.size()) {
        return nullptr;
    }
    return ports_[index].get();
}

void Component::addPort(Port* port) {
    if (port) {
        ports_.emplace_back(port);
    }
}

std::string Component::toString() const {
    return "Component[" + std::to_string(id_) + "] " + name_;
}

} // namespace CircuitSim
