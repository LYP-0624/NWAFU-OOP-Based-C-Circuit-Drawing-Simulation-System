#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H

#include <string>
#include <vector>

namespace CircuitSim {

// 求解器统计信息
struct SolverStats {
    int matrixSize;       // 矩阵尺寸
    int iterations;       // 迭代次数
    bool converged;       // 是否收敛
    std::string errorMessage; // 错误信息
};

// 线性方程组求解器(高斯消元法)
class LinearSolver {
public:
    static bool solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x);
    static bool solveWithStats(std::vector<std::vector<double>>& A, std::vector<double>& b,
                               std::vector<double>& x, SolverStats& stats);

private:
    static bool gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b); // 高斯消元
    static bool partialPivoting(std::vector<std::vector<double>>& A, std::vector<double>& b, int col); // 部分主元选取
    static bool isSingular(double val, double threshold = 1e-12); // 判断是否奇异
    static void normalizeRow(std::vector<std::vector<double>>& A, std::vector<double>& b, int row); // 行归一化
};

} // namespace CircuitSim

#endif
