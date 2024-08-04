#pragma once

#include <functional>

#include <COLA.hh>

namespace config_builder {

    using Float = double;
    using PropertyGetter = std::function<Float(const cola::Particle&)>;

} // namespace config_builder
