#pragma once

#include <memory>

#include <G4FermiBreakUpVI.hh>
#include <G4Fragment.hh>
#include <G4FragmentVector.hh>
#include <G4Evaporation.hh>
#include <G4PhotonEvaporation.hh>
#include <G4StatMF.hh>
#include <XMLConfigModule/lib/condition/builders/registry.h>
#include <XMLConfigModule/lib/model/registry.h>
#include <XMLConfigModule/lib/util/context.h>
#include <XMLConfigModule/lib/util/util.h>

#include "util.h"

namespace wrapper {
    using namespace config_builder;

    template <typename ModelType>
    class G4Model : public IModel {
    public:
        G4Model(std::unique_ptr<ModelType>&& model) : model_(std::move(model)) {}

        cola::EventParticles BreakItUp(const cola::Particle& particle) override {
            cola::EventParticles result;
            auto fragment = ColaToG4Fragment(particle);

            std::unique_ptr<G4FragmentVector> fragments;
            if constexpr (has_break_it_up_v<ModelType>) {
                fragments.reset(model_->BreakItUp(fragment));
            } else {
                fragments = std::make_unique<G4FragmentVector>();
                model_->BreakFragment(fragments.get(), &fragment);
            }

            DEFER {
                if (fragments != nullptr) {
                    for (auto fragment_ptr: *fragments) {
                        delete fragment_ptr;
                    }
                }
            };

            if (fragments == nullptr || fragments->size() <= 1) {
                result.push_back(particle);
            } else {
                for (auto fragment_ptr: *fragments) {
                    result.emplace_back(G4FragmentToCola(*fragment_ptr));
                }
            }

            return result;
        }

    private:
        std::unique_ptr<ModelType> model_;
    };
} // namespace wrapper


REGISTER_MODEL(geant4_stafMF, new wrapper::G4Model(std::make_unique<G4StatMF>()));
REGISTER_MODEL(geant4_fermi, new wrapper::G4Model(std::make_unique<G4FermiBreakUpVI>()));
REGISTER_MODEL(geant4_photon_evaporation, new wrapper::G4Model(std::make_unique<G4PhotonEvaporation>()));
REGISTER_MODEL(geant4_evaporation, new wrapper::G4Model(std::make_unique<G4Evaporation>(new G4PhotonEvaporation())));
