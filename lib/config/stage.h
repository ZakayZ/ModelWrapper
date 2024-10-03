#pragma once

#include <memory>
#include <optional>
#include <variant>

#include <COLA.hh>

#include "lib/condition/iface.h"
#include "lib/model/iface.h"

namespace config_builder {

    class BaseStage;
    class DrainStage;
    class MockStage;
    using Stage = std::variant<
        BaseStage,
        DrainStage,
        MockStage
    >;

    class BaseStage {
    public:
        bool Propogate(const cola::Particle& particle);

        using StageCondition = std::optional<Condition>;
        using StageModel = std::optional<std::reference_wrapper<IModel>>;
        using NextStages = std::vector<std::reference_wrapper<Stage>>;

        StageCondition condition;
        StageModel model;
        NextStages stages;

    private:
        void PushNext(const cola::Particle& particle);
    };

    class DrainStage {
    public:
        bool Propogate(const cola::Particle& particle);

        cola::EventParticles result;
    };

    class MockStage {
    public:
        bool Propogate(const cola::Particle& particle);
    };

} // namespace config_builder
