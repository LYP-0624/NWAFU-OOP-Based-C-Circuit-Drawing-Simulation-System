#include "LinearSolver.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace CircuitSim {

bool LinearSolver::solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x) {
    SolverStats stats;
    return solveWithStats(A, b, x, stats);
}

bool LinearSolver::solveWithStats(std::vector<std::vector<double>>& A, std::vector<double>& b, 
                                   std::vector<double>& x, SolverStats& stats) {
    stats.matrixSize = static_cast<int>(A.size());
    stats.iterations = 0;
    stats.converged = false;
    stats.errorMessage.clear();

    if (A.empty() || b.empty()) {
        stats.errorMessage = "Empty matrix or vector";
        return false;
    }

    int n = static_cast<int>(A.size());
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

    if (!gaussElimination(A, b)) {
        stats.errorMessage = "Singular matrix or no unique solution";
        return false;
    }

    x.resize(n);
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
        stats.iterations++;
    }

    stats.converged = true;
    return true;
}

bool LinearSolver::solveSparse(const std::vector<std::vector<std::pair<int, double>>>& sparseA, 
                                std::vector<double>& b, std::vector<double>& x) {
    int n = static_cast<int>(sparseA.size());
    std::vector<std::vector<double>> A(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        for (const auto& [j, val] : sparseA[i]) {
            if (j >= 0 && j < n) {
                A[i][j] = val;
            }
        }
    }

    return solve(A, b, x);
}

bool LinearSolver::gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b) {
    int n = static_cast<int>(A.size());

    for (int i = 0; i < n; ++i) {
        if (!partialPivoting(A, b, i)) {
            return false;
        }

        if (isSingular(A[i][i])) {
            return false;
        }

        normalizeRow(A, b, i);

        for (int k = i + 1; k < n; ++k) {
            double factor = A[k][i];
            if (std::abs(factor) < 1e-15) continue;

            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    return true;
}

bool LinearSolver::partialPivoting(std::vector<std::vector<double>>& A, std::vector<double>& b, int col) {
    int n = static_cast<int>(A.size());
    double maxVal = std::abs(A[col][col]);
    int maxRow = col;

    for (int k = col + 1; k < n; ++k) {
        double val = std::abs(A[k][col]);
        if (val > maxVal) {
            maxVal = val;
            maxRow = k;
        }
    }

    if (maxVal < 1e-12) {
        return false;
    }

    if (maxRow != col) {
        std::swap(A[col], A[maxRow]);
        std::swap(b[col], b[maxRow]);
    }

    return true;
}

bool LinearSolver::isSingular(double val, double threshold) {
    return std::abs(val) < threshold;
}

void LinearSolver::normalizeRow(std::vector<std::vector<double>>& A, std::vector<double>& b, int row) {
    double pivot = A[row][row];
    if (std::abs(pivot) < 1e-15) return;

    int n = static_cast<int>(A.size());
    for (int j = row + 1; j < n; ++j) {
        A[row][j] /= pivot;
    }
    b[row] /= pivot;
    A[row][row] = 1.0;
}

} // namespace CircuitSim