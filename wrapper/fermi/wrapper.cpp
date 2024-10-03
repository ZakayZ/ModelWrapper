#include "wrapper.h"

#include <FermiBreakUp/Utilities/NucleiProperties/NucleiProperties.h>

using namespace wrapper;

namespace {
    fermi::Particle ColaToFermi(const cola::Particle& particle) {
        auto [A, Z] = particle.getAZ();
        auto mass = MassNumber(A);
        auto charge = ChargeNumber(Z);

        return fermi::Particle(
            mass,
            charge,
            LorentzVector(
                particle.momentum.x,
                particle.momentum.y,
                particle.momentum.z,
                particle.momentum.e
            )
        );
    }

    cola::Particle FermiToCola(const fermi::Particle& particle) {
        return cola::Particle{
            .position=cola::LorentzVector{.t=0, .x=0, .y=0, .z=0},
            .momentum=cola::LorentzVector{
                .e=particle.GetMomentum().e(),
                .x=particle.GetMomentum().x(),
                .y=particle.GetMomentum().y(),
                .z=particle.GetMomentum().z(),
            },
            .pdgCode=cola::AZToPdg({static_cast<int>(particle.GetMassNumber()), static_cast<int>(particle.GetChargeNumber())}),
            .pClass=cola::ParticleClass::produced,
        };
    }
} // anonymous namespace

cola::EventParticles FermiWrapper::BreakItUp(const cola::Particle& particle) {
    cola::EventParticles results;

    for (const auto& product: model_->BreakItUp(ColaToFermi(particle))) {
        results.emplace_back(FermiToCola(product));
    }

    return results;
}
