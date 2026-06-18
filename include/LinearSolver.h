#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H

#include <string>
#include <vector>

namespace CircuitSim {

struct SolverStats {
    int matrixSize;
    int iterations;
    bool converged;
    std::string errorMessage;
};

class LinearSolver {
public:
    static bool solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x);
    static bool solveWithStats(std::vector<std::vector<double>>& A, std::vector<double>& b,
                               std::vector<double>& x, SolverStats& stats);

private:
    static bool gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b);
    static bool partialPivoting(std::vector<std::vector<double>>& A, std::vector<double>& b, int col);
    static bool isSingular(double val, double threshold = 1e-12);
    static void normalizeRow(std::vector<std::vector<double>>& A, std::vector<double>& b, int row);
};

} // namespace CircuitSim

#endif
