#include "lib/config/module.h"

#include "filter.h"

using namespace config_builder;
using namespace cola;

ConfigConverter::ConfigConverter(std::unique_ptr<config_builder::Module>&& model)
    : model_(std::move(model)) {}

std::unique_ptr<cola::EventData> ConfigConverter::operator()(std::unique_ptr<cola::EventData>&& event) {
    cola::EventParticles result;
    for (const auto& particle : event->particles) {
        auto fragments = model_->BreakItUp(particle);
        result.insert(
            result.end(),
            std::make_move_iterator(fragments.begin()),
            std::make_move_iterator(fragments.end())
        );
    }

    event->particles = std::move(result);

    return std::move(event);
}
