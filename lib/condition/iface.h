#pragma once

#include <functional>

#include <COLA.hh>

#include "util/types.h"

namespace config_builder {

    using Condition = std::function<bool(const cola::Particle&)>;

    class ICondition {
    public:
        virtual bool operator()(const cola::Particle& particle) = 0;
        virtual ~ICondition() = default;
    };

} // namespace config_builder
