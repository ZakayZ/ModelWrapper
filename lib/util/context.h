#pragma once

#include <variant>
#include <vector>

#include "lib/condition/iface.h"
#include "lib/model/iface.h"
#include "lib/config/stage.h"

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
