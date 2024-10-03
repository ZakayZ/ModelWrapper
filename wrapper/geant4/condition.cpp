#include <G4NistManager.hh>
#include <Randomize.hh>

#include "condition.h"
#include "util.h"

using namespace wrapper;
using namespace config_builder;

namespace {
    bool EndsWith(const std::string& s, const std::string& suffix) {
        return suffix.size() < s.size() && s.substr(s.size() - suffix.size()) == suffix;
    }

    double StodWithFactor(const std::string& value) {
        auto num = std::stod(value);
        if (EndsWith(value, "eV")) {
            return num * CLHEP::eV;
        }

        if (EndsWith(value, "MeV")) {
            return num * CLHEP::MeV;
        }

        if (EndsWith(value, "keV")) {
            return num * CLHEP::keV;
        }

        if (EndsWith(value, "GeV")) {
            return num * CLHEP::GeV;
        }

        return num;
    }
} // anonymous namespace

Condition StableConditionBuilder::Build(const Context& context) {
    double threshold = 0;
    if (auto it = context.parameters.find("threshold"); it != context.parameters.end()) {
        threshold = StodWithFactor(it->second);
    }

    return [
        nist=G4NistManager::Instance(),
        stable_threshold=threshold
    ](const cola::Particle& particle) -> bool {
        auto nist = G4NistManager::Instance();
        auto [A, Z] = particle.getAZ();
        return A <= 1
               || (GetExcitationEnergy(particle) < stable_threshold
                   && nist->GetIsotopeAbundance(Z, A) > 0);
    };
}

struct FermiParameters {
    int mass_threshold = 19;
    int charge_threshold = 9;
};

Condition FermiConditionBuilder::Build(const Context& context) {
    FermiParameters parameters;
    if (auto it = context.parameters.find("A"); it != context.parameters.end()) {
        parameters.mass_threshold = std::stoi(it->second);
    }
    if (auto it = context.parameters.find("Z"); it != context.parameters.end()) {
        parameters.charge_threshold = std::stoi(it->second);
    }

    return [
        mass_threshold=parameters.mass_threshold,
        charge_threshold=parameters.charge_threshold
    ](const cola::Particle& particle) -> bool {
        auto [A, Z] = particle.getAZ();
        return A <= mass_threshold && Z <= charge_threshold;
    };
}

struct StatMFParameters {
    int mass_threshold = 19;
    int charge_threshold = 9;

    double lower_transition_bound = 3 * CLHEP::MeV;
    double upper_transition_bound = 5 * CLHEP::MeV;
};

Condition StatMFConditionBuilder::Build(const Context& context) {
    StatMFParameters parameters;
    if (auto it = context.parameters.find("A"); it != context.parameters.end()) {
        parameters.mass_threshold = std::stoi(it->second);
    }
    if (auto it = context.parameters.find("Z"); it != context.parameters.end()) {
        parameters.charge_threshold = std::stoi(it->second);
    }
    if (auto it = context.parameters.find("lower_transition"); it != context.parameters.end()) {
        parameters.lower_transition_bound = StodWithFactor(it->second);
    }
    if (auto it = context.parameters.find("upper_transition"); it != context.parameters.end()) {
        parameters.upper_transition_bound = StodWithFactor(it->second);
    }

    return [
        mass_threshold=parameters.mass_threshold,
        charge_threshold=parameters.charge_threshold,
        lower_transition_bound=parameters.lower_transition_bound,
        upper_transition_bound=parameters.upper_transition_bound
    ](const cola::Particle& particle) -> bool {
        auto [A, Z] = particle.getAZ();
        G4double Ex = GetExcitationEnergy(particle);
        if (A < mass_threshold && Z < charge_threshold) {
            return false;
        }

        G4double E = 1 / (2. * (upper_transition_bound - lower_transition_bound));
        G4double E0 = (upper_transition_bound + lower_transition_bound) / 2.;
        G4double w = G4RandFlat::shoot();
        G4double transF = 0.5 * std::tanh((Ex / A - E0) / E) + 0.5;

        if (Ex < lower_transition_bound * A) {
            return false;
        }

        if (w < transF && Ex < upper_transition_bound * A) {
            return true;
        }

        if (w > transF && Ex < upper_transition_bound * A) {
            return false;
        }

        if (Ex > upper_transition_bound * A) {
            return true;
        }

        return false;
    };
}

Condition EvaporationConditionBuilder::Build(const Context& /* context */) {
    return [](const cola::Particle& /* particle */) -> bool {
        return true;
    };
}

Condition PhotonEvaporationConditionBuilder::Build(const Context& /* context */) {
    return [](const cola::Particle& /* particle */) -> bool {
        return true;
    };
}
