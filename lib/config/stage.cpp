#include "stage.h"

using namespace config_builder;

bool BaseStage::Propogate(const cola::Particle& particle) {
    if (condition.has_value() && !condition.value()(particle)) {
        return false;
    }

    if (model.has_value()) {
        for (const auto& particle: model.value().get().BreakItUp(particle)) {
            PushNext(particle);
        }
    } else {
        PushNext(particle);
    }

    return true;
}

void BaseStage::PushNext(const cola::Particle& particle) {
    for (auto stage: stages) {
        auto is_visisted = std::visit(
            [&](auto& child_stage) -> bool {
                return child_stage.Propogate(particle);
            },
            stage.get()
        );
        if (is_visisted) {
            return;
        }
    }

    // TODO better message
    throw std::runtime_error("no stage was satisfied");
}

bool DrainStage::Propogate(const cola::Particle& particle) {
    result.push_back(particle);
    return true;
}
