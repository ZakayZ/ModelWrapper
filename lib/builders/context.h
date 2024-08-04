#pragma once

#include <variant>
#include <vector>

#include "condition/iface.h"
#include "model/iface.h"
#include "stage.h"

namespace config_builder {

    using Tag = std::string;

    using ConfigEntity = std::variant<
        std::reference_wrapper<Stage>,
        std::reference_wrapper<IModel>,
        Condition,
        nullptr_t
    >;

    struct Context {
        Tag tag;
        Params attributes;
        Params parameters;
        std::string contents;
        std::vector<ConfigEntity> children;
    };

} // namespace config_builder
