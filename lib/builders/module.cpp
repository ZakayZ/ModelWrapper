#include <string>
#include <unordered_set>

#include "lib/condition/builders/registry.h"
#include "lib/condition/iface.h"
#include "lib/condition/registry.h"
#include "lib/config/stage.h"
#include "lib/model/builders/registry.h"
#include "lib/model/iface.h"
#include "lib/model/registry.h"
#include "lib/util/util.h"

#include "module.h"

using namespace xml_impl;
using namespace config_builder;

Module ModuleBuilder::Build(const XMLNode& node) {
    if (GetTag(node) != "module") {
        throw std::runtime_error("xml root must be \"module\" tag");
    }

    auto attributes = BuildAttributes(node);

    const auto& intitial_stage_name = attributes.at("initial_stage");
    const auto& drain_stage_name = attributes.at("drain_stage");

    ModuleData data;
    data.stages.Register(drain_stage_name, DrainStage());
    auto& drain = std::get<DrainStage>(data.stages.Find(drain_stage_name).value().get());

    auto visiter = ModuleVisiter(data);
    TraverseModuleTree(node, visiter);

    // must be defined
    auto& init = std::get<BaseStage>(data.stages.Find(intitial_stage_name).value().get());

    for (const auto& [name, stage]: data.stages) {
        if (std::holds_alternative<MockStage>(stage)) {
            std::runtime_error("stage \"" + name + "\" wasn't defined");
        }
    }

    return Module(std::move(data), init, drain);
}

ConfigEntity ModuleVisiter::BuildEntity(const Context& context) {
    if (context.tag == "module") {
        return nullptr;
    }

    if (context.tag == "stage") {
        return BuildStage(context);
    }

    if (context.tag == "condition") {
        return BuildCondition(context);
    }

    if (context.tag == "model") {
        return BuildModel(context);
    }

    if (auto builder = ConditionBuilderRegistry::Find(context.tag); builder.has_value()) {
        return builder->get().Build(context);
    }

    throw std::runtime_error("unknown tag \"" + context.tag + "\"");
}

std::reference_wrapper<Stage> ModuleVisiter::BuildStage(const Context& context) {
    if (context.tag != "stage") {
        throw std::runtime_error("build stage recieved invalid tag \"" + context.tag + "\"");
    }

    if (auto link_it = context.attributes.find("link"); link_it != context.attributes.end()) {
        auto stage = data_.stages.Find(link_it->second);
        if (!stage.has_value()) {
            // create mock stage
            data_.stages.Register(link_it->second, MockStage());
            stage = data_.stages.Find(link_it->second);
        }

        return stage->get();
    }

    const auto& name = context.attributes.at("name");  // it must exist

    if (auto stage = data_.stages.Find(name); stage.has_value()) {
        if (!std::holds_alternative<MockStage>(stage->get())) {
            throw std::runtime_error("stage \"" + name + "\" defined twice");
        }

        stage->get() = BaseStage();
    } else {
        data_.stages.Register(name, BaseStage());
    }

    Stage& stage = data_.stages.Find(name).value().get();
    BaseStage& base_stage = std::get<BaseStage>(stage);

    for (const auto& child: context.children) {
        std::visit(overloaded{
            [&](std::reference_wrapper<Stage> child_stage) {
                base_stage.stages.emplace_back(child_stage);
            },
            [&](const Condition& condition) {
                if (base_stage.condition.has_value()) {
                    throw std::runtime_error("multiple condition tags in stage \"" + name + "\"");
                }
                base_stage.condition = condition;
            },
            [&](std::reference_wrapper<IModel> model) {
                if (base_stage.model.has_value()) {
                    throw std::runtime_error("multiple model tags in stage \"" + name + "\"");
                }
                base_stage.model = model;
            },
            [&](nullptr_t /* arg */) {
                throw std::runtime_error("multiple module elements in config");
            }
        }, child);
    }

    return stage;
}

Condition ModuleVisiter::BuildCondition(const Context& context) {
    if (context.tag != "condition") {
        throw std::runtime_error("build condition recieved invalid tag \"" + context.tag + "\"");
    }

    if (auto link_it = context.attributes.find("link"); link_it != context.attributes.end()) {
        auto condition = data_.conditions.Find(link_it->second);
        if (!condition.has_value()) {
            // TODO did you mean msg
            throw std::runtime_error("no linked condition found \"" + link_it->second + "\"");
        }

        return condition->get();
    }

    if (auto build_it = context.attributes.find("build"); build_it != context.attributes.end()) {
        auto builder = ConditionBuilderRegistry::Find(build_it->second);
        if (!builder.has_value()) {
            // TODO did you mean msg
            throw std::runtime_error("no condition builder found \"" + build_it->second + "\"");
        }

        return builder->get().Build(context);
    }

    if (auto source_it = context.attributes.find("source"); source_it != context.attributes.end()) {
        auto condition = ConditionRegistry::Find(source_it->second);
        if (!condition.has_value()) {
            // TODO: did you mean msg
            throw std::runtime_error("no condition source found \"" + source_it->second + "\"");
        }

        return condition->get();
    }

    const auto& name = context.attributes.at("name");  // it must exist

    if (context.children.size() != 1 || !std::holds_alternative<Condition>(context.children[0])) {
        throw std::runtime_error("condition \"" + name + "\" must contain exactly one condition child");
    }

    if (auto is_new = data_.conditions.Register(name, std::get<Condition>(context.children[0])); !is_new) {
        throw std::runtime_error("duplicate condition: \"" + name + "\"");
    }

    return data_.conditions.Find(name)->get();
}

std::reference_wrapper<IModel> ModuleVisiter::BuildModel(const Context& context) {
    if (context.tag != "model") {
        throw std::runtime_error("build model recieved invalid tag \"" + context.tag + "\"");
    }

    if (auto build_it = context.attributes.find("build"); build_it != context.attributes.end()) {
        auto builder = ModelBuilderRegistry::Find(build_it->second);
        if (!builder.has_value()) {
            throw std::runtime_error("no model builder found: \"" + build_it->second + "\"");
        }
        auto model = builder->get().Build(context);
        data_.models.emplace_back(model);
        return *model;
    }

    if (auto source_it = context.attributes.find("source"); source_it != context.attributes.end()) {
        auto model = ModelRegistry::Find(source_it->second);
        if (!model.has_value()) {
            throw std::runtime_error("no model source found: \"" + source_it->second + "\"");
        }

        return *model;
    }

    throw std::runtime_error("model tag must contain \"build\" or \"source\" attributes");
}
