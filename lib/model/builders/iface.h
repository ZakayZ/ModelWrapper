#pragma once

#include "builders/context.h"
#include "model/iface.h"

namespace config_builder {

    class IModelBuilder {
    public:
        virtual IModel* Build(const Context& context) = 0;

        virtual ~IModelBuilder() = default;
    };

} // namespace config_builder
