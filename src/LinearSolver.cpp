#include "LinearSolver.h"

#include <algorithm>
#include <cmath>

namespace CircuitSim {

bool LinearSolver::solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x) {
    SolverStats stats;
    return solveWithStats(A, b, x, stats);
}

// 带统计信息的求解
bool LinearSolver::solveWithStats(std::vector<std::vector<double>>& A, std::vector<double>& b,
                                  std::vector<double>& x, SolverStats& stats) {
    stats.matrixSize = static_cast<int>(A.size());
    stats.iterations = 0;
    stats.converged = false;
    stats.errorMessage.clear();

    // 输入验证
    if (A.empty() || b.empty()) {
        stats.errorMessage = "Empty matrix or vector";
        return false;
    }

    const int n = static_cast<int>(A.size());
    if (static_cast<int>(b.size()) != n) {
        stats.errorMessage = "Matrix and vector size mismatch";
        return false;
    }

    for (int i = 0; i < n; ++i) {
        if (static_cast<int>(A[i].size()) != n) {
            stats.errorMessage = "Matrix is not square";
            return false;
        }
    }

    // 高斯消元
    if (!gaussElimination(A, b)) {
        stats.errorMessage = "Singular matrix or no unique solution";
        return false;
    }

    // 回代求解
    x.assign(n, 0.0);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        if (isSingular(A[i][i])) {
            stats.errorMessage = "Zero pivot encountered";
            return false;
        }
        x[i] /= A[i][i];
        ++stats.iterations;
    }

    stats.converged = true;
    return true;
}

// 高斯消元
bool LinearSolver::gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b) {
    const int n = static_cast<int>(A.size());

    for (int i = 0; i < n; ++i) {
        // 部分主元选取
        if (!partialPivoting(A, b, i)) {
            return false;
        }

        if (isSingular(A[i][i])) {
            return false;
        }

        // 行归一化
        normalizeRow(A, b, i);

        // 消元
        for (int k = i + 1; k < n; ++k) {
            const double factor = A[k][i];
            if (std::abs(factor) < 1e-15) {
                continue;
            }

            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    return true;
}

// 部分主元选取
bool LinearSolver::partialPivoting(std::vector<std::vector<double>>& A, std::vector<double>& b, int col) {
    const int n = static_cast<int>(A.size());
    double maxVal = std::abs(A[col][col]);
    int maxRow = col;

    // 找到最大值行
    for (int k = col + 1; k < n; ++k) {
        const double val = std::abs(A[k][col]);
        if (val > maxVal) {
            maxVal = val;
            maxRow = k;
        }
    }

    // 检查是否奇异
    if (maxVal < 1e-12) {
        return false;
    }

    // 交换行
    if (maxRow != col) {
        std::swap(A[col], A[maxRow]);
        std::swap(b[col], b[maxRow]);
    }

    return true;
}

bool LinearSolver::isSingular(double val, double threshold) {
    return std::abs(val) < threshold;
}

// 行归一化
void LinearSolver::normalizeRow(std::vector<std::vector<double>>& A, std::vector<double>& b, int row) {
    const double pivot = A[row][row];
    if (std::abs(pivot) < 1e-15) {
        return;
    }

    const int n = static_cast<int>(A.size());
    for (int j = row + 1; j < n; ++j) {
        A[row][j] /= pivot;
    }
    b[row] /= pivot;
    A[row][row] = 1.0;
}

} // namespace CircuitSim
