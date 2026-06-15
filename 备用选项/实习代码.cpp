#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <cstdlib>

using namespace std;

// ===================== 一、元器件抽象基类 & 子类 =====================
class Component
{
protected:
    string name;
    string type;
    int id;
    bool isPowerOn;
    double current;
    double voltage;

public:
    Component(string t, string n, int id)
        : type(t), name(n), id(id), isPowerOn(false), current(0), voltage(0) {}
    virtual ~Component() {} // 移除了 =default，兼容旧编译器

    virtual void run() = 0;

    void setPower(bool on)
    {
        isPowerOn = on;
        if (!on)
        {
            current = 0;
            voltage = 0;
        }
    }

    void setElecParam(double i, double u)
    {
        if (isPowerOn)
        {
            current = i;
            voltage = u;
        }
    }

    string getName() const { return name; }
    string getType() const { return type; }
    int getId() const { return id; }
    bool getPowerState() const { return isPowerOn; }
    double getCurrent() const { return current; }
    double getVoltage() const { return voltage; }
};

// 电源
class Power : public Component
{
private:
    double emf;
public:
    Power(string name, int id, double v)
        : Component("power", name, id), emf(v) {}

    void run()
    {
        if (isPowerOn)
            cout << "[电源] " << name << " 工作，电动势：" << emf << " V" << endl;
        else
            cout << "[电源] " << name << " 已断电" << endl;
    }

    double getEmf() const { return emf; }
};

// 电阻
class Resistor : public Component
{
private:
    double resistance;
public:
    Resistor(string name, int id, double r)
        : Component("resistor", name, id), resistance(r) {}

    void run()
    {
        if (isPowerOn)
            cout << "[电阻] " << name << " 阻值：" << resistance
                 << " Ω，电流：" << current << " A" << endl;
        else
            cout << "[电阻] " << name << " 已断电" << endl;
    }

    double getResistance() const { return resistance; }
};

// 小灯泡
class Light : public Component
{
private:
    bool isLightUp;
public:
    Light(string name, int id)
        : Component("light", name, id), isLightUp(false) {}

    void run()
    {
        isLightUp = isPowerOn;
        if (isLightUp)
            cout << "[灯泡] " << name << " 点亮，电压：" << voltage << " V" << endl;
        else
            cout << "[灯泡] " << name << " 熄灭" << endl;
    }
};

// 电流表
class Ammeter : public Component
{
public:
    Ammeter(string name, int id)
        : Component("ammeter", name, id) {}

    void run()
    {
        cout << "[电流表] " << name << " 读数：" << current << " A" << endl;
    }
};

// 电压表
class Voltmeter : public Component
{
public:
    Voltmeter(string name, int id)
        : Component("voltmeter", name, id) {}

    void run()
    {
        cout << "[电压表] " << name << " 读数：" << voltage << " V" << endl;
    }
};

// 导线类
class Wire
{
public:
    int startId;
    int endId;
    Wire(int s, int e) : startId(s), endId(e) {}
};

// ===================== 二、电路管理器（拓扑 + 仿真求解） =====================
class CircuitManager
{
private:
    vector<Component*> compList;
    vector<Wire> wireList;

public:
    CircuitManager() {}
    ~CircuitManager()
    {
        for (size_t i = 0; i < compList.size(); ++i)
            delete compList[i];
        compList.clear();
    }

    void addComponent(Component* comp)
    {
        if (comp) compList.push_back(comp);
    }

    void addWire(const Wire& wire)
    {
        wireList.push_back(wire);
    }

    // 串联电路求解（基尔霍夫定律）
    void solveCircuit()
    {
        double totalVolt = 0.0;
        double totalRes = 0.0;

        for (size_t i = 0; i < compList.size(); ++i)
        {
            Component* comp = compList[i];
            if (!comp->getPowerState()) continue;

            if (comp->getType() == "power")
            {
                Power* p = dynamic_cast<Power*>(comp);
                totalVolt = p->getEmf();
            }
            else if (comp->getType() == "resistor")
            {
                Resistor* r = dynamic_cast<Resistor*>(comp);
                totalRes += r->getResistance();
            }
            else if (comp->getType() == "light")
            {
                totalRes += 10.0; // 灯泡等效电阻
            }
        }

        if (totalRes < 1e-6)
        {
            cout << "电路断路或总电阻为0，无法求解！" << endl;
            return;
        }

        double current = totalVolt / totalRes;
        cout << "\n===== 电路仿真计算结果 =====" << endl;
        cout << "总电压：" << totalVolt << " V" << endl;
        cout << "总电阻：" << totalRes << " Ω" << endl;
        cout << "支路电流：" << current << " A\n" << endl;

        // 赋值电流、电压
        for (size_t i = 0; i < compList.size(); ++i)
        {
            Component* comp = compList[i];
            if (!comp->getPowerState()) continue;

            if (comp->getType() == "resistor")
            {
                Resistor* r = dynamic_cast<Resistor*>(comp);
                comp->setElecParam(current, current * r->getResistance());
            }
            else if (comp->getType() == "light")
            {
                comp->setElecParam(current, current * 10.0);
            }
            else if (comp->getType() == "ammeter")
            {
                comp->setElecParam(current, 0);
            }
            else if (comp->getType() == "voltmeter")
            {
                comp->setElecParam(0, totalVolt);
            }
        }
    }

    void runAllComponent()
    {
        for (size_t i = 0; i < compList.size(); ++i)
        {
            compList[i]->run();
        }
    }

    const vector<Component*>& getComponents() const { return compList; }
    const vector<Wire>& getWires() const { return wireList; }
};

// ===================== 三、文件工具类（文本持久化、校验、日志、导出） =====================
class FileUtil
{
public:
    // 序列化：保存电路到文本文件
    static bool saveToFile(const string& filePath, const CircuitManager& circuit)
    {
        ofstream fout(filePath.c_str());
        if (!fout.is_open())
        {
            cerr << "错误：无法创建文件 " << filePath << endl;
            return false;
        }

        // 写入元件数量、导线数量、元件参数
        const vector<Component*>& comps = circuit.getComponents();
        const vector<Wire>& wires = circuit.getWires();

        fout << comps.size() << endl;
        for (size_t i = 0; i < comps.size(); ++i)
        {
            Component* c = comps[i];
            fout << c->getType() << " " << c->getName() << " " << c->getId()
                 << " " << c->getPowerState() << " " << c->getCurrent() << " " << c->getVoltage() << endl;
        }

        fout << wires.size() << endl;
        for (size_t i = 0; i < wires.size(); ++i)
        {
            const Wire& w = wires[i];
            fout << w.startId << " " << w.endId << endl;
        }

        fout.close();
        cout << "成功：电路数据已保存至 " << filePath << endl;
        return true;
    }

    // 反序列化 + 合法性校验：加载电路
    static bool loadFromFile(const string& filePath, CircuitManager& circuit)
    {
        ifstream fin(filePath.c_str());
        if (!fin.is_open())
        {
            cerr << "错误：文件 " << filePath << " 不存在" << endl;
            return false;
        }

        try
        {
            int compNum, wireNum;
            fin >> compNum;
            if (compNum < 0)
            {
                cerr << "错误：文件格式非法，数据异常" << endl;
                fin.close();
                return false;
            }

            // 读取元件
            for (int i = 0; i < compNum; ++i)
            {
                string type, name;
                int id;
                bool power;
                double cur, vol;
                fin >> type >> name >> id >> power >> cur >> vol;

                Component* comp = NULL;
                if (type == "power") comp = new Power(name, id, 12.0);
                else if (type == "resistor") comp = new Resistor(name, id, 20.0);
                else if (type == "light") comp = new Light(name, id);
                else if (type == "ammeter") comp = new Ammeter(name, id);
                else if (type == "voltmeter") comp = new Voltmeter(name, id);

                if (comp)
                {
                    comp->setPower(power);
                    comp->setElecParam(cur, vol);
                    circuit.addComponent(comp);
                }
            }

            // 读取导线
            fin >> wireNum;
            for (int i = 0; i < wireNum; ++i)
            {
                int s, e;
                fin >> s >> e;
                circuit.addWire(Wire(s, e));
            }
        }
        catch (...)
        {
            cerr << "错误：文件损坏，解析失败" << endl;
            fin.close();
            return false;
        }

        fin.close();
        cout << "成功：电路文件加载完成" << endl;
        return true;
    }

    // 写入运行日志
    static void writeLog(const string& logPath, const string& msg)
    {
        ofstream f(logPath.c_str(), ios::app);
        time_t now = time(NULL);
        f << "[" << ctime(&now) << "] " << msg << endl;
        f.close();
    }

    // 导出仿真结果报表
    static bool exportResult(const string& filePath, const CircuitManager& circuit)
    {
        ofstream fout(filePath.c_str());
        if (!fout.is_open()) return false;

        fout << "===== 电路图仿真结果报表 =====" << endl;
        const vector<Component*>& comps = circuit.getComponents();
        for (size_t i = 0; i < comps.size(); ++i)
        {
            Component* comp = comps[i];
            fout << "元件：" << comp->getName()
                 << "  电流：" << comp->getCurrent() << "A"
                 << "  电压：" << comp->getVoltage() << "V" << endl;
        }
        fout.close();
        cout << "成功：仿真结果已导出至 " << filePath << endl;
        return true;
    }
};

// ===================== 四、主函数（程序入口 & 整体测试） =====================
int main()
{
    cout << "========== 简易电路图生成与仿真系统 ==========\n" << endl;

    // 1. 创建电路管理器
    CircuitManager circuit;

    // 2. 创建各类元器件
    Power* power = new Power("直流电源", 1, 12.0);
    Resistor* r = new Resistor("限流电阻", 2, 20.0);
    Light* bulb = new Light("小灯泡", 3);
    Ammeter* am = new Ammeter("电流表", 4);
    Voltmeter* vm = new Voltmeter("电压表", 5);

    // 全部通电
    power->setPower(true);
    r->setPower(true);
    bulb->setPower(true);
    am->setPower(true);
    vm->setPower(true);

    // 3. 添加元件与导线，构建电路拓扑
    circuit.addComponent(power);
    circuit.addComponent(r);
    circuit.addComponent(bulb);
    circuit.addComponent(am);
    circuit.addComponent(vm);
    circuit.addWire(Wire(1, 2));
    circuit.addWire(Wire(2, 3));

    // 4. 电路仿真求解
    circuit.solveCircuit();

    // 5. 多态执行元件工作逻辑
    circuit.runAllComponent();

    // 6. 数据持久化：保存 + 加载（含合法性校验）
    FileUtil::saveToFile("circuit_data.txt", circuit);
    CircuitManager newCircuit;
    FileUtil::loadFromFile("circuit_data.txt", newCircuit);

    // 7. 日志记录 + 结果导出
    FileUtil::writeLog("system.log", "电路仿真正常运行，完成一次计算");
    FileUtil::exportResult("sim_result.txt", circuit);

    cout << "\n========== 系统运行结束 ==========" << endl;
    system("pause");
    return 0;
}
