#pragma once

#include "registry.h"

namespace config_builder {

    template <typename Value>
    class StaticRegistry {
    public:
        using Record = typename Registry<Value>::Record;

        StaticRegistry() = delete;
        StaticRegistry(const StaticRegistry&) = delete;
        StaticRegistry(StaticRegistry&&) = delete;

        template <typename... Args>
        static bool Register(const std::string& name, Args&&... args) {
            return StaticRegistry::Instance().Register(name, std::forward<Args>(args)...);
        }

        static std::optional<Record> Find(const std::string& name) {
            return StaticRegistry::Instance().Find(name);
        }

        static void Unregister(const std::string& name) {
            StaticRegistry::Instance().Unregister(name);
        }

    private:
        static Registry<Value>& Instance() {
            static Registry<Value> registry;
            return registry;
        }
    };

} // namespace config_builder
