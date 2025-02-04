#pragma once

#include <COLA.hh>

#include "lib/registry/static_registry.h"
#include "iface.h"

namespace config_builder {
    using PropertyRegistry = StaticRegistry<PropertyGetter>;
} // namespace config_builder

#define REGISTER_PROPERTY(name, value) \
[[maybe_unused]] static bool is_property_defined_##name = config_builder::PropertyRegistry::Register(#name, value);

REGISTER_PROPERTY(A, [](const cola::Particle& particle) { return cola::pdgToAZ(particle.pdgCode).first; });
REGISTER_PROPERTY(Z, [](const cola::Particle& particle) { return cola::pdgToAZ(particle.pdgCode).second; });
