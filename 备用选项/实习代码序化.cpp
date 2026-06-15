bool FileJsonUtil::saveCircuitToJson(const std::string& filePath, const Circuit& circuit)
{
    LOG_INFO("开始保存电路至文件: " + filePath);
    std::ofstream fout(filePath);
    if (!fout.is_open())
    {
        LOG_ERROR("无法创建文件: " + filePath);
        return false;
    }

    json root;
    root["version"] = "1.0";

    // 遍历所有元件，转为JSON数组
    json compArr = json::array();
    auto& compMap = circuit.getComponentsMap();
    for (auto& pair : compMap)
    {
        Component* comp = pair.second;
        json cj;
        cj["id"] = comp->getId();
        cj["type"] = ComponentFactory::typeToString(comp->getType());
        cj["x"] = comp->getX();
        cj["y"] = comp->getY();

        // 保存元件电气参数（电压、电流、阻值等）
        auto props = comp->getAllProperties();
        json propJson;
        for (auto& p : props)
            propJson[p.first] = p.second;
        cj["properties"] = propJson;

        compArr.push_back(cj);
    }
    root["components"] = compArr;

    // 连线数组（预留扩展）
    json connArr = json::array();
    root["connections"] = connArr;

    // dump(4)：格式化JSON，带缩进，方便人工查看
    fout << root.dump(4);
    fout.close();

    LOG_INFO("电路保存成功: " + filePath);
    return true;
}

bool FileJsonUtil::loadCircuitFromJson(const std::string& filePath, Circuit& circuit)
{
    LOG_INFO("开始加载电路文件: " + filePath);
    std::ifstream fin(filePath);
    if (!fin.is_open())
    {
        LOG_ERROR("文件不存在或无法打开: " + filePath);
        return false;
    }

    json root;
    // 捕获JSON解析异常（文件乱码、非JSON、损坏）
    try
    {
        fin >> root;
    }
    catch (const json::parse_error& e)
    {
        LOG_ERROR("JSON解析失败，文件损坏: " + std::string(e.what()));
        fin.close();
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("读取文件异常: " + std::string(e.what()));
        fin.close();
        return false;
    }

    // 第一层校验：整体JSON结构
    if (!checkJsonRootValid(root))
    {
        fin.close();
        return false;
    }

    // 清空原有电路，防止新旧数据混杂
    circuit.clear();
    LOG_INFO("清空原有电路数据，开始重建元件");

    // 遍历元件数组，重建元件对象
    auto compArr = root["components"];
    for (auto& cj : compArr)
    {
        // 第二层校验：单个元件
        if (!checkComponentJsonValid(cj))
            continue;

        int id = cj["id"].get<int>();
        std::string type = cj["type"].get<std::string>();
        double x = cj["x"].get<double>();
        double y = cj["y"].get<double>();

        // 工厂模式创建对应元件（灯泡/电阻/电表）
        Component* comp = ComponentFactory::getInstance().create(type, id, x, y);
        if (!comp)
        {
            LOG_WARN("无法创建元件，跳过: " + type);
            continue;
        }
        circuit.addComponent(comp);
        LOG_INFO("成功加载元件 ID:" + std::to_string(id) + " 类型:" + type);
    }

    // 遍历连线（预留扩展）
    auto connArr = root["connections"];
    for (auto& cn : connArr)
    {
        if (!checkConnectionJsonValid(cn))
            continue;
        LOG_INFO("加载一条连线数据");
    }

    fin.close();
    LOG_INFO("电路文件加载完成");
    return true;
}
