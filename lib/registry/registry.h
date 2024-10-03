#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace config_builder {

    template <typename Value>
    class Registry {
    private:
        // pointers will be deleted by the container
        using InnerValue = std::conditional_t<
                                std::is_pointer_v<Value>,
                                std::unique_ptr<std::remove_pointer_t<Value>>,
                                Value
                            >;
        using Container = std::unordered_map<std::string, InnerValue>;

    public:
        using Record = std::reference_wrapper<std::remove_pointer_t<Value>>;

        using iterator = Container::iterator;
        using const_iterator = Container::const_iterator;

        iterator begin() { return registry_.begin(); }
        iterator end() { return registry_.end(); }

        const_iterator begin() const { return registry_.cbegin(); }
        const_iterator end() const { return registry_.cend(); }

        const_iterator cbegin() const { return registry_.cbegin(); }
        const_iterator cend() const { return registry_.cend(); }

        Registry() = default;
        Registry(Registry&&) = default;
        Registry(const Registry&) = delete;

        Registry& operator=(Registry&&) = default;
        Registry& operator=(const Registry&) = delete;

        template <typename... Args>
        bool Register(const std::string& name, Args&&... args) {
            auto [_, is_emplaced] = registry_.try_emplace(name, std::forward<Args>(args)...);
            return is_emplaced;
        }

        void Unregister(const std::string& name) {
            registry_.erase(name);
        }

        std::optional<Record> Find(const std::string& name) {
            auto it = registry_.find(name);
            if (it != registry_.end()) {
                if constexpr (std::is_pointer_v<Value>) {
                    return *it->second;
                } else {
                    return it->second;
                }
            }

            return {};
        }

        std::optional<const Record> Find(const std::string& name) const {
            auto it = registry_.find(name);
            if (it != registry_.end()) {
                if constexpr (std::is_pointer_v<Value>) {
                    return *it->second;
                } else {
                    return it->second;
                }
            }

            return {};
        }

        size_t Size() const {
            return registry_.size();
        }

    private:
        Container registry_;
    };

} // namespace config_builder
