#pragma once

#include <string>

#include "lib/util/context.h"
#include "lib/util/types.h"
#include "lib/xml_parser/src/pugixml.hpp"

namespace xml_impl {
    using namespace config_builder;

    using XMLNode = pugi::xml_node;

    Params BuildParams(const XMLNode& node);

    Params BuildAttributes(const XMLNode& node);

    Tag GetTag(const XMLNode& node);

    class BaseVisiter {
    public:
        virtual ConfigEntity BuildEntity(const Context& context) = 0;
        virtual ~BaseVisiter() = default;
    };

    ConfigEntity TraverseModuleTree(const XMLNode& node, BaseVisiter& visiter);

}  // namespace xml_impl
