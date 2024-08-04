#include <string_view>
#include <unordered_set>

#include "xml.h"

using namespace xml_impl;
using namespace config_builder;

static const std::unordered_set<std::string> HelperTags = {
    "",      // text node
    "param", // param node
};

Params xml_impl::BuildParams(const XMLNode& node) {
    Params params;
    for (const auto& child: node.children()) {
        if (GetTag(child) == "param") {
            auto name = child.attribute("name");
            if (name.empty()) {
                throw std::runtime_error("\"param\" tag must contain \"name\" attribute");
            }
            auto value = child.text().get();
            auto [_, is_new] = params.try_emplace(name.value(), value);
            if (!is_new) {
                throw std::runtime_error("multiple \"param\" tags with same name: \"" + std::string(name.value()) + "\"");
            }
        }
    }

    return params;
}

Params xml_impl::BuildAttributes(const XMLNode& node) {
    Params params;
    for (const auto& attribute: node.attributes()) {
        params[attribute.name()] = attribute.value();
    }

    return params;
}

Tag xml_impl::GetTag(const XMLNode& node) {
    return node.name();
}

ConfigEntity xml_impl::TraverseModuleTree(const XMLNode& node, BaseVisiter& visiter) {
    auto context = Context{
        .tag=GetTag(node),
        .attributes=BuildAttributes(node),
        .parameters=BuildParams(node),
        .contents=node.text().get(),
        .children={},
    };

    for (const auto& child: node.children()) {
        if (child.empty() || HelperTags.contains(GetTag(child))) {
            continue;
        }
        context.children.emplace_back(TraverseModuleTree(child, visiter));
    }

    return visiter.BuildEntity(context);
}
