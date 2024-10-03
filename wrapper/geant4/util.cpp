#include <optional>

#include <G4NucleiProperties.hh>

#include "util.h"

using namespace wrapper;

G4Fragment wrapper::ColaToG4Fragment(const cola::Particle& particle) {
    auto [A, Z] = particle.getAZ();

    return G4Fragment(
        G4int(A),
        G4int(Z),
        G4LorentzVector(
            particle.momentum.x,
            particle.momentum.y,
            particle.momentum.z,
            particle.momentum.e
        )
    );
}

cola::Particle wrapper::G4FragmentToCola(const G4Fragment& fragment) {
    return cola::Particle{
        .position=cola::LorentzVector{.t=0, .x=0, .y=0, .z=0},
        .momentum=cola::LorentzVector{
            .e=fragment.GetMomentum().e(),
            .x=fragment.GetMomentum().x(),
            .y=fragment.GetMomentum().y(),
            .z=fragment.GetMomentum().z(),
        },
        .pdgCode=cola::AZToPdg({fragment.GetA_asInt(), fragment.GetZ_asInt()}),
        .pClass=cola::ParticleClass::produced,
    };
}

double wrapper::GetMass(const cola::Particle& particle) {
    auto [A, Z] = particle.getAZ();

    return G4NucleiProperties::GetNuclearMass(A, Z);
}

double wrapper::GetExcitationEnergy(const cola::Particle& particle) {
    auto mag2 = particle.momentum.mag2();
    if (mag2 < 0) {
        mag2 = 0;
    }
    return std::sqrt(mag2) - GetMass(particle);
}
