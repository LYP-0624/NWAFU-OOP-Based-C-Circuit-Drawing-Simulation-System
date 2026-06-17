#include "CircuitSolver.h"
#include "PowerSource.h"
#include <cmath>
// 构造函数：一开始电闸当然是断开的
CircuitSolver::CircuitSolver() {
    isClosed = false;
}

// 析构函数：负责在软件关闭时，把背包里的元件内存全部释放掉，防止电脑卡死
CircuitSolver::~CircuitSolver() {
    ClearCircuit();
}

// 添加元件：利用 vector 的 push_back 绝招，直接塞进背包尾部
void CircuitSolver::AddComponent(Component* comp) {
    if (comp != nullptr) {
        components.push_back(comp);
    }
}

// 清空电路：释放内存并清空背包
void CircuitSolver::ClearCircuit() {
    for (auto comp : components) {
        delete comp; // 销毁实体的内存
    }
    components.clear(); // 清空背包名单
}

// 拨动电闸
void CircuitSolver::SetSwitch(bool closed) {
    isClosed = closed;
    // 💡 高分联动：只要电闸状态一变，立马触发一次全电路重新计算！
    CalculateCircuit(); 
}

// ==========================================
// 下面是 CalculateCircuit() 的实现区域
// ==========================================
// 记得在 CircuitSolver.cpp 的最上面（#include "CircuitSolver.h" 之后）加上这行：
// #include "PowerSource.h"

// ==========================================
// 🔥 物理引擎的终极必杀技：全电路求解计算
// ==========================================

   // ---------------------------------------------------------
// ⚔️ 物理引擎核心数学库：高斯消元法解矩阵方程
// ---------------------------------------------------------
std::vector<double> CircuitSolver::SolveLinearSystem(std::vector<std::vector<double>>& A, std::vector<double>& b) {
    int n = A.size();
    std::vector<double> x(n, 0.0);

    for (int i = 0; i < n; i++) {
        // 寻找主元（保证数学计算不崩溃）
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                maxRow = k;
            }
        }
        std::swap(A[i], A[maxRow]);
        std::swap(b[i], b[maxRow]);

        // 消元
        for (int k = i + 1; k < n; k++) {
            if (std::abs(A[i][i]) < 1e-12) continue; // 防止除以0
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; j++) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }

    // 回代求解
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (int j = i + 1; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        if (std::abs(A[i][i]) > 1e-12) {
            x[i] = (b[i] - sum) / A[i][i];
        }
    }
    return x;
}

// 找最大节点号（决定矩阵是几乘几的）
int CircuitSolver::GetMaxNode() {
    int max_node = 0;
    for (auto comp : components) {
        if (comp->GetNode1() > max_node) max_node = comp->GetNode1();
        if (comp->GetNode2() > max_node) max_node = comp->GetNode2();
    }
    return max_node;
}

// ==========================================
// 🔥 终极必杀技：基于节点导纳矩阵的全电路求解
// ==========================================
void CircuitSolver::CalculateCircuit() {
    if (!isClosed) {
        // 没通电，全员电流电压归零
        for (auto comp : components) {
            comp->SetVoltage(0.0);
            comp->SetCurrent(0.0);
            comp->UpdateState(); 
        }
        return;
    }

    int n = GetMaxNode();
    if (n == 0) return; // 只有地线(0号节点)，不用算

    // 1. 初始化 N x N 的节点导纳矩阵(Y) 和 注入电流向量(I)
    std::vector<std::vector<double>> Y(n, std::vector<double>(n, 0.0));
    std::vector<double> I(n, 0.0);

    // 2. 遍历所有元件，往矩阵里“盖印章”(Matrix Stamping)
    for (auto comp : components) {
        int n1 = comp->GetNode1();
        int n2 = comp->GetNode2();
        
        PowerSource* ps = dynamic_cast<PowerSource*>(comp);
        double g = 0.0; // 电导 = 1 / 电阻

        if (ps) {
            // 💡 高分物理技巧：电池怎么进矩阵？用“诺顿等效电路”！
            // 把电池看作一个巨大恒流源和极小内阻(0.001欧)的并联
            double r = 1e-3; 
            g = 1.0 / r;
            // 假设正极连 n1，负极连 n2，注入电流 = 电压 / 内阻
            double current_inject = ps->GetVoltage() / r; 
            if (n1 > 0) I[n1 - 1] += current_inject; // 流入 n1
            if (n2 > 0) I[n2 - 1] -= current_inject; // 流出 n2
        } else {
            // 普通元件（电阻、灯泡等）
            double res = comp->GetResistance();
            if (res < 1e-6) res = 1e-6; // 防止短路导线除以0
            g = 1.0 / res;
        }

        // 把电导 G 填入 Y 矩阵对应的位置
        if (n1 > 0) Y[n1 - 1][n1 - 1] += g;
        if (n2 > 0) Y[n2 - 1][n2 - 1] += g;
        if (n1 > 0 && n2 > 0) {
            Y[n1 - 1][n2 - 1] -= g;
            Y[n2 - 1][n1 - 1] -= g;
        }
    }

    // 3. 放狗！用高斯消元法瞬间解出矩阵： Y * V = I
    std::vector<double> V = SolveLinearSystem(Y, I);

    // 4. 把算出来的节点电压，分发给每一个元件
    for (auto comp : components) {
        int n1 = comp->GetNode1();
        int n2 = comp->GetNode2();
        
        // 拿到两端电压（如果是0号节点就是地，电势为0）
        double v1 = (n1 > 0) ? V[n1 - 1] : 0.0;
        double v2 = (n2 > 0) ? V[n2 - 1] : 0.0;
        
        // 元件电压 = 两端电势差
        double compVoltage = v1 - v2;
        comp->SetVoltage(compVoltage);
        
        if (!dynamic_cast<PowerSource*>(comp)) {
            // 如果不是电源，就老老实实用 欧姆定律 算自己的电流
            double res = comp->GetResistance();
            if (res > 0) {
                comp->SetCurrent(compVoltage / res);
            }
            // 通知元件更新（灯泡亮起！）
            comp->UpdateState();
        }
    }
}