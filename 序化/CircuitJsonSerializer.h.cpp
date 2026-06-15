#ifndef CIRCUIT_JSON_SERIALIZER_H
#define CIRCUIT_JSON_SERIALIZER_H

#include <string>
#include <nlohmann/json.hpp>
#include "Circuit.h"
#include "CircuitException.h"
#include "SimulationLogger.h"

using json = nlohmann::json;

namespace CircuitSim
{
    class CircuitJsonSerializer
    {
    public:
        // 序列化：电路对象 → 标准JSON
        static json serializeToJson(const Circuit& circuit);
        
        // 反序列化：标准JSON → 电路对象
        static bool deserializeFromJson(const json& json_data, Circuit& circuit);
        
        // 保存电路到本地JSON文件
        static bool saveToJsonFile(const Circuit& circuit, const std::string& file_path);
        
        // 从本地JSON文件加载电路
        static bool loadFromJsonFile(const std::string& file_path, Circuit& circuit);

    private:
        // 校验JSON顶层结构合法性
        static bool validateJsonRootStructure(const json& json_data);
        
        // 校验单个元件JSON结构与参数合法性
        static bool validateComponentJson(const json& comp_json);
        
        // 校验节点与连线关系合法性
        static bool validateConnectionRelations(const json& json_data);
        
        // 元件对象 → JSON
        static json serializeComponent(const Component* component);
        
        // 节点对象 → JSON
        static json serializeNode(const Node* node);
        
        // 支路/连线对象 → JSON
        static json serializeBranch(const Branch* branch);
        
        // JSON → 元件对象
        static Component* deserializeComponent(const json& comp_json);
        
        // JSON → 节点对象
        static Node* deserializeNode(const json& node_json);
        
        // JSON → 支路/连线对象
        static Branch* deserializeBranch(const json& branch_json, Circuit& circuit);
    };
}

#endif // CIRCUIT_JSON_SERIALIZER_H
