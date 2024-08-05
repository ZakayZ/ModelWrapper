#pragma once

#include "iface.h"
#include "lib/registry/static_registry.h"

namespace config_builder {

    using ModelBuilderRegistry = StaticRegistry<IModelBuilder*>;

    #define REGISTER_MODEL_BUILDER(name, value) \
    [[maybe_unused]] static const bool is_model_builder_defined_##name = ModelBuilderRegistry::Register(#name, value);

} // namespace config_builder
