#ifndef CIRCUIT_FILE_VALIDATOR_H
#define CIRCUIT_FILE_VALIDATOR_H

#include <string>

#include "json.hpp"

using json = nlohmann::json;  // 为nlohmann::json定义别名，方便使用

namespace CircuitSim {

// 电路文件验证器类，提供静态方法验证JSON电路文件
class CircuitFileValidator {
public:
    // 验证文件路径是否合法（非空、.json后缀、读取时文件存在）
    static bool validateFilePath(const std::string& file_path, bool for_writing);
    // 验证文件内容是否为合法JSON格式（检查首字符是否为{或[）
    static bool validateJsonFileFormat(const std::string& file_path);
    // 验证单个元器件的JSON参数是否符合类型要求
    static bool validateComponentParameters(const json& comp_json);
};

} // namespace CircuitSim

#endif // CIRCUIT_FILE_VALIDATOR_H