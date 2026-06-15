// 新增测试函数：电路JSON持久化测试
void testCircuitJsonPersistence()
{
    printSeparator();
    std::cout << "Test 6: Circuit JSON Persistence (Save/Load)" << std::endl;
    printSeparator();

    // 1. 创建测试电路
    Circuit test_circuit;
    test_circuit.setName("Test Series Circuit");

    PowerSource* ps = new PowerSource(1, 0, 0, 9.0);
    Resistor* r1 = new Resistor(2, 100, 0, 150.0);
    Bulb* bulb = new Bulb(3, 200, 0, 10.0, 0.5);
    Ammeter* am = new Ammeter(4, 50, 50);
    Voltmeter* vm = new Voltmeter(5, 150, 50);

    ps->setPower(true);
    r1->setPower(true);
    bulb->setPower(true);
    am->setPower(true);
    vm->setPower(true);

    test_circuit.addComponent(ps);
    test_circuit.addComponent(r1);
    test_circuit.addComponent(bulb);
    test_circuit.addComponent(am);
    test_circuit.addComponent(vm);

    // 连接电路
    test_circuit.connect(ps->getPorts()[0], r1->getPorts()[0]);
    test_circuit.connect(r1->getPorts()[1], bulb->getPorts()[0]);
    test_circuit.connect(bulb->getPorts()[1], ps->getPorts()[1]);
    test_circuit.connect(am->getPorts()[0], ps->getPorts()[0]);
    test_circuit.connect(am->getPorts()[1], r1->getPorts()[0]);
    test_circuit.connect(vm->getPorts()[0], r1->getPorts()[0]);
    test_circuit.connect(vm->getPorts()[1], r1->getPorts()[1]);

    // 求解电路
    test_circuit.solve();
    printResults(test_circuit.getResults());

    // 2. 保存电路到JSON文件
    std::string file_path = "test_circuit.json";
    std::cout << "\nSaving circuit to " << file_path << "..." << std::endl;
    if (test_circuit.saveToJsonFile(file_path))
    {
        std::cout << "Circuit saved successfully!" << std::endl;
    }
    else
    {
        std::cout << "Failed to save circuit!" << std::endl;
        return;
    }

    // 3. 新建电路，从JSON文件加载
    Circuit loaded_circuit;
    std::cout << "\nLoading circuit from " << file_path << "..." << std::endl;
    if (loaded_circuit.loadFromJsonFile(file_path))
    {
        std::cout << "Circuit loaded successfully!" << std::endl;
    }
    else
    {
        std::cout << "Failed to load circuit!" << std::endl;
        return;
    }

    // 4. 验证加载结果
    std::cout << "\n=== Loaded Circuit Results ===" << std::endl;
    loaded_circuit.solve();
    printResults(loaded_circuit.getResults());

    // 5. 测试非法文件加载
    printSeparator();
    std::cout << "Testing invalid file load..." << std::endl;
    std::ofstream bad_file("bad_circuit.json");
    bad_file << "{ invalid json }";
    bad_file.close();

    Circuit bad_circuit;
    if (!bad_circuit.loadFromJsonFile("bad_circuit.json"))
    {
        std::cout << "Invalid file load blocked successfully!" << std::endl;
    }
}

// 在main函数中调用测试
int main()
{
    // ... 原有测试代码 ...
    testCircuitJsonPersistence();

    printSeparator();
    std::cout << "All tests completed!" << std::endl;
    printSeparator();

    return 0;
}
