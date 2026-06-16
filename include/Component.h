#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include <map>

namespace CircuitSim {

class Port;

enum class ComponentType {
    RESISTOR,
    POWER_SOURCE,
    BULB,
    AMMETER,
    VOLTMETER,
    SWITCH,
    GROUND,
    CAPACITOR,
    INDUCTOR
};

class Component {
protected:
    int id_;
    std::string name_;
    double x_, y_;
    double rotation_;
    bool selected_;
    std::vector<Port*> ports_;
    std::map<std::string, double> properties_;

public:
    Component(int id, const std::string& name, double x, double y);
    virtual ~Component();

    virtual ComponentType getType() const = 0;
    virtual std::string getTypeName() const;
    virtual void simulate(double voltage, double current) = 0;
    virtual double getProperty(const std::string& key) const = 0;
    virtual std::map<std::string, double> getAllProperties() const = 0;
    virtual Component* clone() const = 0;

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    void setProperty(const std::string& key, double value) { properties_[key] = value; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }
    double getRotation() const { return rotation_; }
    void setRotation(double r) { rotation_ = r; }
    bool isSelected() const { return selected_; }
    void setSelected(bool s) { selected_ = s; }
    
    std::vector<Port*>& getPorts() { return ports_; }
    const std::vector<Port*>& getPorts() const { return ports_; }
    
    void addPort(Port* port);
    
    virtual std::string toString() const;
};

} // namespace CircuitSim

#endif