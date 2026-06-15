#include "LinearSolver.h"
#include <cmath>

namespace CircuitSim {

bool LinearSolver::solve(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x) {
    if (!gaussElimination(A, b)) return false;
    
    int n = A.size();
    x.resize(n);
    
    for (int i = n - 1; i >= 0; --i) {
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        if (std::abs(A[i][i]) < 1e-10) return false;
        x[i] /= A[i][i];
    }
    return true;
}

bool LinearSolver::gaussElimination(std::vector<std::vector<double>>& A, std::vector<double>& b) {
    int n = A.size();
    for (int i = 0; i < n; ++i) {
        double maxVal = std::abs(A[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; ++k) {
            if (std::abs(A[k][i]) > maxVal) {
                maxVal = std::abs(A[k][i]);
                maxRow = k;
            }
        }
        if (maxVal < 1e-10) return false;
        
        if (maxRow != i) {
            std::swap(A[i], A[maxRow]);
            std::swap(b[i], b[maxRow]);
        }
        
        for (int k = i + 1; k < n; ++k) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    return true;
}

} // namespace CircuitSim