#include <yaml-cpp/yaml.h>

#include <iostream>
using namespace std;

bool ReadTestYaml()
{
    YAML::Node root = YAML::LoadFile("config/yaml/test.yaml");

    if (root["server_id"])
    {
        uint32_t server_id = root["server_id"].as<uint32_t>();
        cout << "server_id :" << server_id << endl;
    }

    if (root["languages"])
    {
        for (YAML::const_iterator iter = root["languages"].begin(); iter != root["languages"].end(); ++iter)
        {
            if (iter->first && iter->second)
            {
                std::string language = iter->first.as<std::string>();
                uint32_t id          = iter->second.as<uint32_t>();

                cout << "language: " << language << "  id: " << id << endl;
            }
        }
    }

    return true;
}

int main(void)
{
    ReadTestYaml();

    return 0;
}