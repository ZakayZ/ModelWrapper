#pragma once

#include "module.h"
#include "xml.h"

namespace config_builder {

    class ModuleBuilder {
    public:
        Module Build(const xml_impl::XMLNode& node);
    };

    class ModuleVisiter : public xml_impl::BaseVisiter {
    public:
        ModuleVisiter(ModuleData& data) : data_(data) {}

        ConfigEntity BuildEntity(const Context& context) override;

    private:
        std::reference_wrapper<Stage> BuildStage(const Context& context);
        Condition BuildCondition(const Context& context);
        std::reference_wrapper<IModel> BuildModel(const Context& context);

        ModuleData& data_;
    };

} // namespace config_builder
