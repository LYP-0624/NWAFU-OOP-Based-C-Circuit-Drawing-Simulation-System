#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H

#include <vector>

namespace CircuitSim {

class LinearSolver {
public:
    static bool solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x);
    
private:
    static bool gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b);
};

} // namespace CircuitSim

#endif