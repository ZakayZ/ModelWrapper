#include <iostream>
#include <map>

#include <config_module.h>

#include "wrapper.h"
#include "condition.h"
#include "util.h"

using namespace wrapper;
using namespace config_builder;

int main() {
    auto params = std::map<std::string, std::string>({{"config_file", "config.xml"}});
    auto factory = cola::ConfigFactory();
    auto converter = std::unique_ptr<cola::VConverter>(dynamic_cast<cola::VConverter*>(factory.create(params)));
    auto data = std::make_unique<cola::EventData>();
    cola::Particle particle;
    particle.pdgCode = cola::AZToPdg({15, 14});
    particle.momentum.x = 100000;
    data->particles.push_back(particle);
    auto results = (*converter)(std::move(data));
    for (const auto& el : results->particles) {
        std::cout << el.pdgCode << '\n';
    }
    
    std::cout << config_builder::ModelRegistry::Find("geant4_stafMF").has_value() << '\n';
    std::cout << "adasddas\n";
}
