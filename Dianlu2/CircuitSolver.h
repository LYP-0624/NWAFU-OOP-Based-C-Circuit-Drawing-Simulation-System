#ifndef CIRCUITSOLVER_H
#define CIRCUITSOLVER_H

#include <vector>
#include "Component.h"
#include "PowerSource.h"

class CircuitSolver {
private:
    std::vector<Component*> components; 
    bool isClosed; 

    // 👇新增：物理引擎的数学武器库
    int GetMaxNode(); // 找出当前电路里最大的节点编号
    // 高斯消元法解线性方程组 (Y * V = I)
    std::vector<double> SolveLinearSystem(std::vector<std::vector<double>>& A, std::vector<double>& b);

public:
    CircuitSolver();
    ~CircuitSolver();

    void AddComponent(Component* comp); 
    void ClearCircuit();                 
    void SetSwitch(bool closed);         

    // 终极算法入口
    void CalculateCircuit(); 
};

#endif // CIRCUITSOLVER_H