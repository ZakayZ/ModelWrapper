#pragma once

#include <XMLConfigModule/lib/condition/builders/registry.h>
#include <XMLConfigModule/lib/util/context.h>

namespace wrapper {
    using namespace config_builder;

    class StableConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context);
    };

    class FermiConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context);
    };

    class StatMFConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context);
    };

    class EvaporationConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context);
    };

    class PhotonEvaporationConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context);
    };
} // namespace wrapper

REGISTER_CONDITION_BUILDER(stable, new wrapper::StableConditionBuilder());
REGISTER_CONDITION_BUILDER(fermi_condition, new wrapper::FermiConditionBuilder());
REGISTER_CONDITION_BUILDER(statMF_condition, new wrapper::StatMFConditionBuilder());
REGISTER_CONDITION_BUILDER(evaporation_condition, new wrapper::EvaporationConditionBuilder());
REGISTER_CONDITION_BUILDER(photon_evaporation_condition, new wrapper::PhotonEvaporationConditionBuilder());
