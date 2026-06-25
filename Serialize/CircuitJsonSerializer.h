#ifndef CIRCUIT_JSON_SERIALIZER_H
#define CIRCUIT_JSON_SERIALIZER_H

#include <string>

#include "json.hpp"

#include "Branch.h"
#include "Circuit.h"
#include "CircuitException.h"
#include "Node.h"
#include "SimulationLogger.h"

using json = nlohmann::json;

namespace CircuitSim {

// 电路JSON序列化器
class CircuitJsonSerializer {
public:
    static json serializeToJson(const Circuit& circuit);  // 序列化为JSON对象
    static bool deserializeFromJson(const json& json_data, Circuit& circuit); // 从JSON恢复电路
    static bool saveToJsonFile(const Circuit& circuit, const std::string& file_path); // 保存到JSON文件
    static bool loadFromJsonFile(const std::string& file_path, Circuit& circuit); // 从JSON文件加载

private:
    static bool validateJsonRootStructure(const json& json_data); // 验证JSON根结构
    static bool validateComponentJson(const json& comp_json);     // 验证元件JSON

    static json serializeComponent(const Component* component);   // 序列化元件
    static json serializeNode(const Node* node);                 // 序列化节点
    static json serializeBranch(const Branch* branch);           // 序列化支路

    static Component* deserializeComponent(const json& comp_json); // 反序列化元件
    static Node* deserializeNode(const json& node_json);           // 反序列化节点
    static Branch* deserializeBranch(const json& branch_json, Circuit& circuit); // 反序列化支路
};

} // namespace CircuitSim

#endif // CIRCUIT_JSON_SERIALIZER_H
