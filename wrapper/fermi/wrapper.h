#pragma once

#include <memory>
#include <XMLConfigModule/lib/model/iface.h>
#include <XMLConfigModule/lib/model/registry.h>

#include <FermiBreakUp/VFermiBreakUp.h>
#include <FermiBreakUp/FermiBreakUp.h>
#include <FermiBreakUp/Configurations/FastConfigurations.h>

namespace wrapper {
    using namespace config_builder;

    class FermiWrapper : public IModel {
    public:
        FermiWrapper(std::unique_ptr<fermi::VFermiBreakUp>&& model) : model_(std::move(model)) {}

        cola::EventParticles BreakItUp(const cola::Particle& particle) override;

    private:
        std::unique_ptr<fermi::VFermiBreakUp> model_; 
    };

} // namespace wrapper

REGISTER_MODEL(fermi, new wrapper::FermiWrapper(
    std::unique_ptr<fermi::VFermiBreakUp>(new fermi::FermiBreakUp(
        std::unique_ptr<fermi::VConfigurations>(new fermi::FastConfigurations())
    ))
));
