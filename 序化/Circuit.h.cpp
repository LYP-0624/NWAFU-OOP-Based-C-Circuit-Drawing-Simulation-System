// 新增JSON持久化核心接口
std::string getName() const;
void setName(const std::string& name);
bool isSolved() const;
const std::vector<SimulationResult>& getSimulationResults() const;

json toJson() const;
bool fromJson(const json& json_data);
bool saveToJsonFile(const std::string& file_path);
bool loadFromJsonFile(const std::string& file_path);
