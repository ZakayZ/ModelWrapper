#pragma once

#include "lib/model/iface.h"
#include "lib/util/context.h"

namespace config_builder {

    class IModelBuilder {
    public:
        virtual IModel* Build(const Context& context) = 0;

        virtual ~IModelBuilder() = default;
    };

} // namespace config_builder
