#include "lib/builders/module.h"

#include "filter.h"
#include "factory.h"

using namespace xml_impl;
using namespace config_builder;
using namespace cola;

cola::VFilter* ConfigFactory::create(const std::map<std::string, std::string>& params) {
    pugi::xml_document doc;
    pugi::xml_parse_result result;

    if (auto it = params.find("config"); it != params.end()) {
        result = doc.load_string(it->second.c_str());
    } else if (auto it = params.find("config_file"); it != params.end()) {
        result = doc.load_file(it->second.c_str());
    } else {
        throw std::runtime_error("no \"config\" or \"config_file\" parameters found");
    }

    if (!result) {
        throw std::runtime_error(std::string("document error: ") + result.description());
    }

    auto builder = ModuleBuilder();

    return new ConfigConverter(std::make_unique<Module>(builder.Build(doc.first_child())));
}
