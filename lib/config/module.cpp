#include "module.h"

using namespace config_builder;

Module::Module(ModuleData&& data, BaseStage& in, DrainStage& out)
    : data_(std::move(data))
    , initial_stage_(in)
    , drain_stage_(out)
{
}

cola::EventParticles Module::BreakItUp(const cola::Particle& particle) {
    drain_stage_.result.clear();
    if (initial_stage_.Propogate(particle)) {
        return std::move(drain_stage_.result);
    }
    throw std::runtime_error("initial stage wasn't satisfied");
}
