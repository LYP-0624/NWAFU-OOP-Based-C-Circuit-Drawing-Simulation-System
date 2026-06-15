#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <string>
#include <map>
#include <functional>

namespace CircuitSim {

class Component;

class ComponentFactory {
public:
    using Creator = std::function<<Component*(int id, double x, double y)>;

    static ComponentFactory& getInstance();

    void registerType(const std::string& typeName, Creator creator);
    Component* create(const std::string& typeName, int id, double x, double y);

    static std::string typeToString(ComponentType type);
    static ComponentType stringToType(const std::string& str);

    void registerBuiltInTypes();

private:
    ComponentFactory() = default;
    std::map<std::string, Creator> creators_;
};

#define REGISTER_COMPONENT(TYPE, CLASS) \
    static bool _reg_##CLASS = [](){ \
        CircuitSim::ComponentFactory::getInstance().registerType(#TYPE, \
            [](int id, double x, double y)->CircuitSim::Component*{ return new CLASS(id, x, y); }); \
        return true; \
    }();

} // namespace CircuitSim

#endif