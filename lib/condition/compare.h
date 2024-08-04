#pragma once

#include <COLA.hh>

#include "iface.h"
#include "property/iface.h"

namespace config_builder {

    template <class Comparator>
    class CompareCondition : public ICondition {
    public:
        CompareCondition(const std::string& value, PropertyGetter& getter) : value_(std::stod(value)), getter_(getter) {}

        bool operator()(const cola::Particle& particle) override {
            return Comparator()(getter_(particle), value_);
        }

    private:
        Float value_;
        PropertyGetter getter_;
    };

} // namespace config_builder
