#include "stage.h"

using namespace config_builder;

// TODO: better compare
bool operator==(const cola::Particle& a, const cola::Particle& b) {
    return a.pdgCode == b.pdgCode;
}

bool BaseStage::Propogate(const cola::Particle& particle) {
    if (condition.has_value() && !condition->operator()(particle)) {
        return false;
    }

    if (model.has_value()) {
        auto fragments = model->get().BreakItUp(particle);
        if (fragments.size() == 0 || (fragments.size() == 1 && fragments.front() == particle)) {
            return false;
        }

        for (const auto& particle: fragments) {
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

bool MockStage::Propogate(const cola::Particle& /* particle */) {
    throw std::runtime_error("mock stage schouldn't be used directly");
}
