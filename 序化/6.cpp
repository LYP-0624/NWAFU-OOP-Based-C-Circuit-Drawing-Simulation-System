// 新增接口实现
std::string Circuit::getName() const
{
    return name_;
}

void Circuit::setName(const std::string& name)
{
    name_ = name;
}

bool Circuit::isSolved() const
{
    return solved_;
}

const std::vector<SimulationResult>& Circuit::getSimulationResults() const
{
    return simulation_results_;
}

json Circuit::toJson() const
{
    return CircuitJsonSerializer::serializeToJson(*this);
}

bool Circuit::fromJson(const json& json_data)
{
    return CircuitJsonSerializer::deserializeFromJson(json_data, *this);
}

bool Circuit::saveToJsonFile(const std::string& file_path)
{
    return CircuitJsonSerializer::saveToJsonFile(*this, file_path);
}

bool Circuit::loadFromJsonFile(const std::string& file_path)
{
    return CircuitJsonSerializer::loadFromJsonFile(file_path, *this);
}
