#include "CircuitFileValidator.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace CircuitSim {

namespace {

// 检查文件路径是否以.json结尾（不区分大小写）
bool endsWithJson(const std::string& path) {
    if (path.size() < 5) {  // .json共5个字符
        return false;
    }
    std::string suffix = path.substr(path.size() - 5);  // 提取最后5个字符
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));  // 统一转小写
    });
    return suffix == ".json";
}

} // namespace

// 验证文件路径是否合法
bool CircuitFileValidator::validateFilePath(const std::string& file_path, bool for_writing) {
    if (file_path.empty() || !endsWithJson(file_path)) {  // 路径为空或非json后缀
        return false;
    }

    if (!for_writing) {  // 读取模式：检查文件是否存在且可读
        std::ifstream in(file_path, std::ios::binary);
        return in.good();
    }

    return true;  // 写入模式：路径合法即可
}

// 验证文件内容是否为合法的JSON格式（检查首字符）
bool CircuitFileValidator::validateJsonFileFormat(const std::string& file_path) {
    std::ifstream in(file_path, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }

    char first = '\0';
    in >> std::ws;  // 跳过前导空白字符
    if (!in.get(first)) {  // 读取第一个非空白字符
        return false;
    }
    return first == '{' || first == '[';  // JSON对象或数组的起始符
}

// 验证单个元器件的JSON参数是否合法
bool CircuitFileValidator::validateComponentParameters(const json& comp_json) {
    if (!comp_json.contains("type") || !comp_json["type"].is_string()) {  // 必须包含type字段且为字符串
        return false;
    }

    const std::string type = comp_json["type"].get<std::string>();
    // 获取properties字段，若不存在则使用空对象
    const auto props = comp_json.contains("properties") && comp_json["properties"].is_object()
        ? comp_json["properties"]
        : json::object();

    if (type == "Resistor" || type == "Bulb") {  // 电阻/灯泡：必须有正电阻值
        return props.contains("resistance") && props["resistance"].is_number() && props["resistance"].get<double>() > 0.0;
    }
    if (type == "PowerSource") {  // 电源：必须有非零电压值
        return props.contains("value") && props["value"].is_number() && props["value"].get<double>() != 0.0;
    }
    if (type == "Switch") {  // 开关：必须有通态和断态电阻
        return props.contains("resistance_on") && props.contains("resistance_off");
    }
    if (type == "Capacitor") {  // 电容：必须有正电容值
        return props.contains("capacitance") && props["capacitance"].is_number() && props["capacitance"].get<double>() > 0.0;
    }
    if (type == "Inductor") {  // 电感：必须有正电感值
        return props.contains("inductance") && props["inductance"].is_number() && props["inductance"].get<double>() > 0.0;
    }

    return true;  // 其他类型默认通过
}

} // namespace CircuitSim