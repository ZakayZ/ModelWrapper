#pragma once

#include <string>

#include <COLA.hh>

#include "util/types.h"

namespace config_builder {

    class IModel {
    public:
        // virtual void Initialize(const Params& /* params */) {}
        virtual cola::EventParticles BreakItUp(const cola::Particle& particle) = 0;
        virtual ~IModel() = default;
    };

} // namespace config_builder
