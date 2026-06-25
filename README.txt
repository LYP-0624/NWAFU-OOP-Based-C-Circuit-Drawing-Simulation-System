================================================================================
                       简易电路图生成与仿真系统
                             README
================================================================================

1. 项目概述
================================================================================

项目名称：简易电路图生成与仿真系统
开发环境：C++17 / Qt 6 / CMake / VS Code
项目性质：西北农林科技大学信息工程学院 面向对象程序设计实习项目
团队规模：3人协作开发

  成员A — 负责 UI（用户界面）设计与实现
  成员B — 负责文件序列化（JSON）与运行日志记录
  成员C — 负责核心求解引擎（MNA/线性求解器）、元器件类构建与代码整合

本系统是一个基于 C++17 和 Qt 6 的图形化直流电路编辑与仿真工具。用户可
以通过拖拽方式在画布上放置电阻、电源、灯泡、电表、电容、电感和开关等
元器件，用导线将它们连接起来，然后运行仿真。系统采用改进节点分析法
（Modified Nodal Analysis, MNA）对直流稳态电路进行求解，并实时将计算
得到的电压、电流、功率等电气量以可视化方式反馈给用户。


2. 功能特性
================================================================================

  [1] 图形化电路编辑
      - 拖拽放置 8 种电路元器件（电阻、电源、灯泡、电流表、电压表、
        电容、电感、开关）
      - 元器件支持旋转、移动、选中、删除
      - 导线连接：从元件端口拖拽导线到另一元件端口，实现智能吸附
      - 每个元器件严格拥有两个端口（左/右），端口以可视化小圆圈表示

  [2] 直流稳态电路仿真
      - 基于改进节点分析法（MNA）构建电路方程
      - 自动建立节点-支路关联矩阵，处理电阻、电压源、电流源等元件
      - 使用高斯消元法（含列主元选取）求解线性方程组
      - 支持含电容、电感的直流稳态分析（稳态下电容视为开路，电感视为短路）

  [3] 实时电气量计算与可视化反馈
      - 仿真完成后，各元件图形上显示电压、电流值
      - 灯泡根据通电状态呈现发光/熄灭效果
      - 电流表、电压表显示实时测量读数
      - 仿真结果以弹窗汇总展示

  [4] 数据持久化
      - 支持将当前电路图保存为 JSON 格式文件
      - 支持从 JSON 文件加载电路图
      - 包含完整的文件格式校验（路径合法性、JSON 结构、元件参数类型）

  [5] 运行日志记录
      - 仿真日志记录器（单例模式），记录每次仿真的结果
      - 支持多种日志级别：DEBUG / INFO / WARNING / ERROR / FATAL
      - 支持导出日志为 CSV 文件
      - 可查询历史仿真数据、按元器件 ID 获取结果

  [6] 电路验证
      - 连通性检测：检查电路是否形成完整回路
      - 短路检测：检查是否存在直接短路
      - 元件参数合法性验证


3. 系统架构
================================================================================

本系统采用经典的三层分层架构：

  ┌─────────────────────────────────────────────────┐
  │                 表示层 (UI)                       │
  │  MainWindow / CircuitScene / GraphComponent /    │
  │  WireItem / componentitems.h                     │
  │  (Qt 6 Widgets / QGraphicsScene)                 │
  ├─────────────────────────────────────────────────┤
  │                 引擎层 (Engine)                   │
  │  MNASolver / LinearSolver / ComponentFactory /   │
  │  CircuitValidator / CircuitJsonSerializer /      │
  │  CircuitFileValidator / SimulationLogger          │
  ├─────────────────────────────────────────────────┤
  │                 数据层 (Data)                     │
  │  Component / 8个派生类 / Node / Port /            │
  │  Branch / Circuit / SimulationResult /            │
  │  CircuitException                                 │
  └─────────────────────────────────────────────────┘

【数据层】
  - Component 抽象基类定义所有元器件的公共接口（getType、simulate、
    getProperty、clone 等），8 个派生类分别实现具体元器件行为。
  - Node 表示电路节点，记录节点电压和连接的端口。
  - Port 表示元器件的连接端口，每个元件有两个端口（左/右）。
  - Branch 表示连接两个节点之间的支路，关联到具体元器件。
  - Circuit 是电路拓扑管理器，负责元件/节点/支路的增删改查、连接管理、
    仿真调度和 JSON 序列化。
  - SimulationResult 结构体存储单个元器件的仿真结果数据。
  - CircuitException 及其派生类提供层次化的异常处理机制。

【引擎层】
  - MNASolver：改进节点分析法求解器，构建导纳矩阵，执行仿真计算。
  - LinearSolver：线性方程组求解器，实现高斯消元法 + 列主元选取。
  - ComponentFactory：单例工厂，通过 REGISTER_COMPONENT 宏自动注册
    所有元器件类型，支持按类型名称字符串创建实例。
  - CircuitValidator：静态电路验证器，提供连通性、短路、元件参数检查。
  - CircuitJsonSerializer：电路-JSON 序列化/反序列化器。
  - CircuitFileValidator：JSON 文件格式校验器。
  - SimulationLogger：单例日志记录器，记录仿真历史和导出日志。

【表示层】
  - MainWindow：主窗口，管理菜单栏、工具栏、状态栏和信号槽连接。
  - CircuitScene：基于 QGraphicsScene 的电路画布，处理鼠标交互、元件
    放置、导线连接等操作。
  - GraphComponent：图形元件基类，绘制各元件的电路符号。
  - WireItem：导线图形项，绘制连接两个端口的导线。
  - componentitems.h：定义 8 种元器件的具体图形绘制类。

【核心设计模式】
  - 抽象基类 + 多态：Component 基类定义统一接口，派生类实现具体行为
  - 单例工厂：ComponentFactory 使用单例模式 + 工厂模式创建元件
  - 并查集拓扑管理：Circuit 中通过并查集合并连接节点，管理电路拓扑
  - MNA 矩阵戳记：MNASolver 使用"戳记法"（Stamping）将元件贡献
    累加到系统导纳矩阵中


4. 目录结构
================================================================================

代码根目录：code/

├── include/            (核心头文件)
│   ├── Component.h          元器件抽象基类及类型枚举
│   ├── Resistor.h           电阻类
│   ├── PowerSource.h        电源类（电压源/电流源）
│   ├── Bulb.h               灯泡类
│   ├── Ammeter.h            电流表类
│   ├── Voltmeter.h          电压表类
│   ├── Switch.h             开关类
│   ├── Capacitor.h          电容类
│   ├── Inductor.h           电感类
│   ├── Circuit.h            电路拓扑管理器
│   ├── MNASolver.h          MNA求解器
│   ├── LinearSolver.h       线性方程求解器
│   ├── Node.h               电路节点
│   ├── Port.h               元件端口
│   ├── Branch.h             电路支路
│   ├── ComponentFactory.h   元件工厂（单例 + REGISTER_COMPONENT宏）
│   ├── CircuitValidator.h   电路验证器
│   ├── CircuitException.h   异常类体系
│   └── SimulationResult.h   仿真结果结构体
│
├── src/                (核心源文件)
│   ├── Component.cpp         元器件基类实现
│   ├── Resistor.cpp          电阻实现
│   ├── PowerSource.cpp       电源实现
│   ├── Bulb.cpp              灯泡实现
│   ├── Ammeter.cpp           电流表实现
│   ├── Voltmeter.cpp         电压表实现
│   ├── Switch.cpp            开关实现
│   ├── Capacitor.cpp         电容实现
│   ├── Inductor.cpp          电感实现
│   ├── Circuit.cpp           电路拓扑管理实现
│   ├── MNASolver.cpp         MNA求解器实现
│   ├── LinearSolver.cpp      线性求解器实现
│   ├── Node.cpp              节点实现
│   ├── Port.cpp              端口实现
│   ├── Branch.cpp            支路实现
│   ├── ComponentFactory.cpp  工厂实现
│   └── CircuitValidator.cpp  验证器实现
│
├── UI/                 (用户界面层)
│   ├── main.cpp              程序入口
│   ├── mainwindow.h / .cpp   主窗口
│   ├── mainwindow.ui         Qt Designer UI 布局文件
│   ├── circuitscene.h / .cpp 电路画布场景（QGraphicsScene）
│   ├── graphcomponent.h / .cpp 图形元件基类
│   ├── componentitems.h      8种元器件的图形绘制类
│   └── wireitem.h / .cpp     导线图形项
│
├── Serialize/          (序列化与日志)
│   ├── CircuitJsonSerializer.h / .cpp  电路-JSON序列化器
│   ├── CircuitFileValidator.h / .cpp   JSON文件校验器
│   └── SimulationLogger.h / .cpp       仿真日志记录器
│
├── lib/                (第三方库)
│   └── nlohmann/
│       └── json.hpp            nlohmann/json 单头文件库
│
├── CMakeLists.txt       CMake 构建配置
├── CircuitSim.pro       Qt qmake 项目文件（备选）
└── build.bat            Windows 批处理构建脚本


5. 核心类说明
================================================================================

5.1 Component（抽象基类）
    - 定义所有元器件的公共接口和属性
    - 属性：id、name、坐标(x,y)、旋转角度、选中状态、端口列表、属性键值对
    - 纯虚接口：getType()、simulate()、getProperty()、getAllProperties()、clone()
    - 8 个派生类：Resistor、PowerSource、Bulb、Ammeter、Voltmeter、
      Switch、Capacitor、Inductor

5.2 ComponentFactory（单例工厂）
    - 单例模式：全局唯一实例，通过 getInstance() 获取
    - 工厂方法：registerType() 注册类型，create() 按名称创建元件
    - REGISTER_COMPONENT 宏：利用静态初始化机制，在程序启动时自动
      将各元件类注册到工厂中，无需手动编写注册代码
    - 支持：typeToString() / stringToType() 类型名与枚举互转

5.3 Circuit（电路拓扑管理器）
    - 管理元件集合（components_）、节点列表（nodes_）、支路列表（branches_）
    - connect() 连接两个端口，内部自动创建/合并节点并建立支路
    - disconnect() 断开端口连接
    - solve() 调用 MNASolver 执行仿真
    - 并查集节点合并：通过 buildConnectivityGraph() 将电气上连通的节点合并
    - JSON 序列化：toJson() / fromJson() / saveToJsonFile() / loadFromJsonFile()
    - 仿真结果查询：queryById() / queryByType()

5.4 MNASolver（改进节点分析法求解器）
    - 建立节点编号映射（buildNodeIndexMap），将节点ID映射到矩阵行/列索引
    - 矩阵戳记（Stamping）方法：
      stampResistor()      — 将电导贡献戳入导纳矩阵
      stampVoltageSource() — 将电压源贡献戳入扩展矩阵
      stampCurrentSource() — 将电流源贡献戳入右端向量
    - assembleMatrix() 遍历所有元件构建系统矩阵
    - solve() 组装矩阵后调用 LinearSolver 求解，再 extractResults() 提取结果

5.5 LinearSolver（线性求解器）
    - 实现高斯消元法（gaussElimination）
    - 列主元选取（partialPivoting）提高数值稳定性
    - 奇异矩阵检测（isSingular），阈值 1e-12
    - 行归一化（normalizeRow）
    - solve() 和 solveWithStats() 两种接口，后者返回求解统计信息

5.6 CircuitValidator（电路验证器）
    - isConnected()：检查电路是否连通（无开路）
    - noShortCircuit()：检查是否存在短路（零电阻回路）
    - validateComponents()：验证各元件参数合法性
    - validate()：综合验证，返回 ValidationResult（含错误信息和警告列表）

5.7 CircuitJsonSerializer（JSON序列化器）
    - serializeToJson()：将 Circuit 对象序列化为 nlohmann::json 对象
    - deserializeFromJson()：从 JSON 对象恢复 Circuit
    - saveToJsonFile() / loadFromJsonFile()：文件读写
    - 内部辅助方法：serializeComponent/Node/Branch、
      deserializeComponent/Node/Branch、validateJsonRootStructure 等

5.8 SimulationLogger（仿真日志记录器）
    - 单例模式，全局唯一日志器
    - logSimulation() 记录每次仿真的结果列表
    - 支持 5 种日志级别：DEBUG / INFO / WARNING / ERROR / FATAL
    - logComponentState() / logCircuitState() 记录状态快照
    - exportCSV() 导出为 CSV 文件
    - getHistory() 按元件ID和属性名获取历史数据

5.9 CircuitException（异常体系）
    - CircuitException          基础异常（继承 std::runtime_error）
    - InvalidConnectionException 无效连接异常
    - OpenCircuitException       开路异常
    - SingularMatrixException    奇异矩阵异常（短路导致）
    - NoGroundException          无接地节点异常
    - InvalidJsonException       JSON 格式无效异常
    - FileIOException            文件读写异常

5.10 其他数据类
    - Node：电路节点，记录节点电压、连接端口、是否接地
    - Port：元件端口，关联到父元件和所连节点
    - Branch：电路支路，关联到两个节点之间的元件，记录电流/电压/功率
    - SimulationResult：仿真结果结构体，含 componentId、电压、电流、功率、
      工作状态、额外属性


6. 构建与运行
================================================================================

6.1 依赖项
    - Qt 6 （Widgets、Gui、Core 模块）
    - nlohmann/json 单头文件库（已包含在 lib/ 目录下）
    - CMake >= 3.16
    - 编译器：支持 C++17（MSVC、MinGW GCC 8+、Clang 等）

6.2 构建命令（CMake 标准流程）

    # 方法一：使用 build.bat 脚本（Windows + MinGW）
    cd code
    build.bat

    # 方法二：手动 CMake 构建
    cd code
    mkdir build && cd build
    cmake .. -G "MinGW Makefiles" ^
        -DCMAKE_PREFIX_PATH="D:/Qt/6.11.1/mingw_64" ^
        -DCMAKE_C_COMPILER="D:/Qt/Tools/mingw1310_64/bin/gcc.exe" ^
        -DCMAKE_CXX_COMPILER="D:/Qt/Tools/mingw1310_64/bin/g++.exe"
    cmake --build . -j 4

    # 方法三：使用 Qt Creator IDE
    直接用 Qt Creator 打开 CMakeLists.txt 或 CircuitSim.pro，
    配置 Qt 6 Kit 后点击构建。

6.3 运行方式
    - 构建成功后，在 build_mingw1310/ 目录下找到 CircuitSim.exe
    - 双击运行，或通过命令行执行：
      cd build_mingw1310
      CircuitSim.exe


7. 使用说明
================================================================================

7.1 基本操作流程
    [1] 放置元件：点击工具栏上的元件按钮（电阻/电源/灯泡/电表/电容/电感/开关），
        然后在画布上点击放置元件。
    [2] 连接导线：鼠标悬停在元件端口的蓝色圆圈上，按住左键拖拽到另一元件的
        端口圆圈上释放，即可建立导线连接。
    [3] 设置参数：双击元件可弹出参数对话框，修改电阻值、电源电压、电容值、
        电感值等参数。双击开关可直接切换闭合/断开状态。
    [4] 运行仿真：点击工具栏"求解"按钮或按 Space 键，系统将执行 MNA 仿真
        计算，并在各元件上显示电压、电流结果。
    [5] 查看结果：灯泡会根据通电状态改变颜色（发光效果）、电流表/电压表
        显示测量读数、仿真结果弹窗汇总展示所有元件的数据。

7.2 快捷键
    - Space           运行仿真
    - Delete          删除选中的元件
    - Ctrl + N        新建电路
    - Ctrl + O        打开电路文件
    - Ctrl + S        保存电路文件

7.3 菜单功能
    - 文件 → 新建：清空当前电路
    - 文件 → 保存：将电路保存为 JSON 文件
    - 文件 → 加载：从 JSON 文件加载电路
    - 文件 → 退出：退出程序
    - 仿真 → 求解电路：执行仿真计算
    - 仿真 → 清除全部：清空画布
    - 帮助 → 使用说明：显示操作帮助
    - 帮助 → 关于：显示版本信息

7.4 支持的元器件
    ┌──────────┬──────────────────────────────────────┐
    │ 元器件    │ 可调参数                              │
    ├──────────┼──────────────────────────────────────┤
    │ 电阻 (R) │ 电阻值 (Ω)                            │
    │ 电源 (V) │ 电压值 (V)，支持电压源/电流源切换      │
    │ 灯泡 (B) │ 电阻值 (Ω)、额定功率 (W)              │
    │ 电流表(A)│ 自动测量（串联在电路中）               │
    │ 电压表(V)│ 自动测量（并联在电路中）               │
    │ 电容 (C) │ 电容值 (F)                            │
    │ 电感 (L) │ 电感值 (H)                            │
    │ 开关 (K) │ 闭合/断开状态（双击切换）              │
    └──────────┴──────────────────────────────────────┘


8. 作者与分工
================================================================================

本项目由西北农林科技大学信息工程学院 3 名学生协作完成，具体分工如下：

  姚家鑫 — UI（用户界面）设计与实现
      负责 MainWindow、CircuitScene、GraphComponent、WireItem 等
      Qt 图形界面的设计与编码，包括菜单栏、工具栏、状态栏、画布交互、
      元件图形绘制、导线连接交互等。

  刘政宜 — 文件序列化与日志记录
      负责 CircuitJsonSerializer（JSON 序列化/反序列化）、
      CircuitFileValidator（文件格式校验）、SimulationLogger
      （日志记录器）等模块的设计与实现。

  李永鹏 — 核心求解引擎与元器件类构建及代码整合
      负责 Component 抽象基类及 8 个派生类的设计与实现、
      ComponentFactory 单例工厂、Circuit 拓扑管理器、
      MNASolver 求解器、LinearSolver 线性求解器、
      CircuitValidator 验证器、Node/Port/Branch 数据类、
      异常体系、以及 CMake 构建配置和代码整合。

================================================================================
                          END OF README
================================================================================