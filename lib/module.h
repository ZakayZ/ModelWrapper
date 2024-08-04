#pragma once

#include <memory>
#include <string>
#include <vector>

#include "condition/iface.h"
#include "model/iface.h"
#include "registry/registry.h"
#include "stage.h"

namespace config_builder {

    struct ModuleData {
        Registry<Stage> stages;
        Registry<Condition> conditions;
        std::vector<std::unique_ptr<IModel>> models;
    };

    class Module : public IModel {
    public:
        Module(ModuleData&& data, BaseStage& in, DrainStage& out);

        Module(const Module&) = delete;
        Module(Module&&) noexcept = default;

        Module& operator=(const Module&) = delete;
        Module& operator=(Module&&) noexcept = default;

        cola::EventParticles BreakItUp(const cola::Particle& particle) override;

    private:
        ModuleData data_;

        BaseStage& initial_stage_;
        DrainStage& drain_stage_;
    };

} // namespace config_builder
