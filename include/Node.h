#ifndef NODE_H
#define NODE_H

#include <vector>

namespace CircuitSim {

class Port;

class Node {
private:
    int id_;
    std::vector<Port*> ports_;
    double voltage_;
    bool isGround_;

public:
    explicit Node(int id);
    ~Node();

    int getId() const { return id_; }
    double getVoltage() const { return voltage_; }
    void setVoltage(double v) { voltage_ = v; }
    bool isGround() const { return isGround_; }
    void setGround(bool g) { isGround_ = g; }

    void addPort(Port* port);
    void removePort(Port* port);
    const std::vector<Port*>& getPorts() const { return ports_; }
    size_t getComponentCount() const;
};

} // namespace CircuitSim

#endif
