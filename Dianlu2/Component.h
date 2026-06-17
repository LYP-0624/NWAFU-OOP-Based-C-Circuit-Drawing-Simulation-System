#ifndef COMPONENT_H
#define COMPONENT_H

// 元器件抽象基类
class Component {
protected:
    int id;               
    double voltage;       
    double current;       
    double resistance;    

    // 👇 就是这俩小家伙，必须在这里！
    int node1; 
    int node2; 

public:
    Component(int comp_id);
    virtual ~Component() {}
    virtual void UpdateState() = 0; 

    int GetId() const { return id; }
    double GetVoltage() const { return voltage; }
    double GetCurrent() const { return current; }
    double GetResistance() const { return resistance; }
    
    // 👇 报错就是因为缺了这三个函数！请确保它们乖乖待在 public: 下面！
    int GetNode1() const { return node1; }
    int GetNode2() const { return node2; }
    void SetNodes(int n1, int n2) { 
        node1 = n1; 
        node2 = n2; 
    }

    void SetVoltage(double v) { voltage = v; }
    void SetCurrent(double c) { current = c; }
};

#endif // COMPONENT_H