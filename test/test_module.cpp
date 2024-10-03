#include <gtest/gtest.h>
#include <map>
#include <optional>

#include "config_module.h"
#include "registry.h"

using namespace config_builder;

namespace {
    using ParamMap = std::map<std::string, std::string>;

    class CutoffConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context) {
            std::optional<int> atomic_threshold;
            std::optional<int> charge_threshold;

            if (auto it = context.parameters.find("A_threshold"); it != context.parameters.end()) {
                atomic_threshold = std::stoi(it->second);
            }

            if (auto it = context.parameters.find("Z_threshold"); it != context.parameters.end()) {
                charge_threshold = std::stoi(it->second);
            }

            return [
                atomic_threshold=atomic_threshold.value_or(2),
                charge_threshold=charge_threshold.value_or(2)
            ](const cola::Particle& particle) -> bool {
                auto [A, Z] = cola::pdgToAZ(particle.pdgCode);
                return A < atomic_threshold && Z < charge_threshold;
            };
        }
    };

    REGISTER_CONDITION_BUILDER(cutoff, new CutoffConditionBuilder());

    class AppendingModel : public IModel {
    public:
        AppendingModel(const cola::Particle& particle) : new_particle_(particle) {}

        cola::EventParticles BreakItUp(const cola::Particle& particle) override {
            return {particle, new_particle_};
        }

    private:
        cola::Particle new_particle_;
    };

    REGISTER_MODEL(append, new AppendingModel({}));
} // anonymous namespace

TEST(TestFactory, TestFactory) {
    auto params = ParamMap({{"config_file", "config.xml"}});
    auto factory = cola::ConfigFactory();
    auto converter = std::unique_ptr<cola::VConverter>(dynamic_cast<cola::VConverter*>(factory.create(params)));

    {
        auto results = (*converter)(std::make_unique<cola::EventData>());
        EXPECT_EQ(results->particles.size(), 0);
    }

    {
        auto data = std::make_unique<cola::EventData>();
        data->particles.push_back({});
        auto results = (*converter)(std::move(data));
        EXPECT_EQ(results->particles.size(), 2);
    }

    {
        auto data = std::make_unique<cola::EventData>();
        cola::Particle particle;
        particle.pdgCode = cola::AZToPdg({5, 3});
        data->particles.push_back(particle);
        auto results = (*converter)(std::move(data));
        EXPECT_EQ(results->particles.size(), 1);
    }
}
