#include "CircuitFileValidator.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace CircuitSim
{
    bool CircuitFileValidator::validateFilePath(const std::string& file_path, bool for_writing)
    {
        if (file_path.empty())
        {
            SimulationLogger::getInstance().logError("File path is empty");
            return false;
        }

        fs::path path(file_path);

        // 校验文件扩展名
        if (path.extension() != ".json")
        {
            SimulationLogger::getInstance().logError("Invalid file extension, must be .json: " + file_path);
            return false;
        }

        if (for_writing)
        {
            // 写入模式：校验目录是否存在，是否可写
            fs::path parent_dir = path.parent_path();
            if (!parent_dir.empty() && !fs::exists(parent_dir))
            {
                SimulationLogger::getInstance().logWarn("Parent directory does not exist, will create: " + parent_dir.string());
                try
                {
                    fs::create_directories(parent_dir);
                }
                catch (const std::exception& e)
                {
                    SimulationLogger::getInstance().logError("Failed to create parent directory: " + std::string(e.what()));
                    return false;
                }
            }
        }
        else
        {
            // 读取模式：校验文件是否存在，是否可读
            if (!fs::exists(path))
            {
                SimulationLogger::getInstance().logError("File does not exist: " + file_path);
                return false;
            }
            if (!fs::is_regular_file(path))
            {
                SimulationLogger::getInstance().logError("Path is not a regular file: " + file_path);
                return false;
            }
        }

        return true;
    }

    bool CircuitFileValidator::validateJsonFileFormat(const std::string& file_path)
    {
        std::ifstream in_file(file_path);
        if (!in_file.is_open())
        {
            SimulationLogger::getInstance().logError("Failed to open file for format validation: " + file_path);
            return false;
        }

        // 校验文件大小（空文件直接拦截）
        in_file.seekg(0, std::ios::end);
        std::streamsize file_size = in_file.tellg();
        in_file.seekg(0, std::ios::beg);

        if (file_size == 0)
        {
            SimulationLogger::getInstance().logError("JSON file is empty: " + file_path);
            return false;
        }

        // 校验文件开头是否为合法JSON格式（必须以{开头）
        char first_char;
        in_file.get(first_char);
        if (first_char != '{')
        {
            SimulationLogger::getInstance().logError("Invalid JSON file format, must start with '{'");
            return false;
        }

        in_file.close();
        return true;
    }

    bool CircuitFileValidator::validateComponentParameters(const json& comp_json)
    {
        std::string type = comp_json["type"];

        // 电阻参数校验
        if (type == "Resistor")
        {
            if (!comp_json["properties"].contains("resistance"))
            {
                SimulationLogger::getInstance().logError("Resistor missing 'resistance' property");
                return false;
            }
            double resistance = comp_json["properties"]["resistance"];
            if (resistance <= 1e-10)
            {
                SimulationLogger::getInstance().logError("Resistor resistance must be greater than 0: " + std::to_string(resistance));
                return false;
            }
        }

        // 电源参数校验
        else if (type == "PowerSource")
        {
            if (!comp_json["properties"].contains("voltage"))
            {
                SimulationLogger::getInstance().logError("PowerSource missing 'voltage' property");
                return false;
            }
            double voltage = comp_json["properties"]["voltage"];
            if (std::abs(voltage) < 1e-10)
            {
                SimulationLogger::getInstance().logError("PowerSource voltage cannot be 0: " + std::to_string(voltage));
                return false;
            }
        }

        // 灯泡参数校验
        else if (type == "Bulb")
        {
            if (!comp_json["properties"].contains("resistance") || !comp_json["properties"].contains("rated_power"))
            {
                SimulationLogger::getInstance().logError("Bulb missing required properties");
                return false;
            }
            double resistance = comp_json["properties"]["resistance"];
            double rated_power = comp_json["properties"]["rated_power"];
            if (resistance <= 1e-10 || rated_power <= 0)
            {
                SimulationLogger::getInstance().logError("Bulb resistance and rated power must be greater than 0");
                return false;
            }
        }

        // 开关参数校验
        else if (type == "Switch")
        {
            if (!comp_json["properties"].contains("resistance_on") || !comp_json["properties"].contains("resistance_off"))
            {
                SimulationLogger::getInstance().logError("Switch missing required properties");
                return false;
            }
            double r_on = comp_json["properties"]["resistance_on"];
            double r_off = comp_json["properties"]["resistance_off"];
            if (r_on < 0 || r_off < 0)
            {
                SimulationLogger::getInstance().logError("Switch resistance cannot be negative");
                return false;
            }
        }

        return true;
    }

    bool CircuitFileValidator::validateCircuitTopology(const json& json_data)
    {
        // 校验是否有至少一个电源
        bool has_power_source = false;
        for (const auto& comp_json : json_data["components"])
        {
            if (comp_json["type"] == "PowerSource")
            {
                has_power_source = true;
                break;
            }
        }
        if (!has_power_source)
        {
            SimulationLogger::getInstance().logWarn("Circuit has no power source, simulation will not work");
        }

        // 校验是否有接地节点
        bool has_ground = false;
        for (const auto& node_json : json_data["nodes"])
        {
            if (node_json["is_ground"])
            {
                has_ground = true;
                break;
            }
        }
        if (!has_ground)
        {
            SimulationLogger::getInstance().logWarn("Circuit has no ground node, simulation may fail");
        }

        return true;
    }
}
