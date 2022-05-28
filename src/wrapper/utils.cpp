//
// Created by qzwxsaedc on 2022/5/26.
//

#include <sys/stat.h>
#include "utils.h"
#include <fstream>
#include <unordered_map>
#include <mutex>

namespace Live2DWrapper {
    static std::once_flag init_flag;

    class LocalResourcesManager {
        std::unordered_map<usize, godot::String> record;
        inline static LocalResourcesManager *_instance{};
        LocalResourcesManager() = default;
        ~LocalResourcesManager() {
            if(!record.empty()) {
                godot_error("memory leak has occurred.");
                for(const auto& iter : record) {
                    auto p = reinterpret_cast<bytes>(iter.first);
                    godot_warning("resource not release: {0}", iter.second);
                    delete[] p;
                }
            }
        }
    public:
        LocalResourcesManager(LocalResourcesManager&) = delete;
        LocalResourcesManager& operator=(const LocalResourcesManager&) = delete;

        inline bytes load_resources(godot::String& path, u32* size) {
            i32 len = 0;
            struct stat stat_buffer{};
            if(stat(path.alloc_c_string(), &stat_buffer) == 0)
                len = stat_buffer.st_size;

            std::fstream file;
            byte* buffer = new byte[len];

            file.open(path.alloc_c_string(), std::ios::in | std::ios::binary);
            if(!file.is_open()) {
                godot_error("cannot open file at {0}.", path);
                return nullptr;
            }

            file.read((char*)buffer, len);
            file.close();
            godot_print("load resource: {0}", path);
            record[reinterpret_cast<usize>(buffer)] = path;
            if(size) *size = len;

            return reinterpret_cast<bytes>(buffer);
        }

        inline void release_resources(bytes p_resources) {
            auto k = reinterpret_cast<usize>(p_resources);
            auto iter = record.find(k);
            if(iter == record.end())
                godot_warning("try release resource created by other way.");
            else {
                godot_print("release resource: {0}", iter->second);
                record.erase(iter);
            }
            delete[] p_resources;
        }

        static LocalResourcesManager* instance() {
            std::call_once(init_flag, []() {
                if(_instance == nullptr)
                    _instance = new LocalResourcesManager();
            });
            return _instance;
        }
    };

    bytes load_file_as_bytes(godot::String path, u32* size) {
        return LocalResourcesManager::instance()->load_resources(path, size);
    }

    void release_bytes(bytes& data) {
        LocalResourcesManager::instance()->release_resources(data);
        data = nullptr;
    }
}