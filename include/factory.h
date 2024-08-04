#pragma once

#include <COLA.hh>

namespace cola {

    class ConfigFactory final: public cola::VFactory {
    public:
        cola::VFilter* create(const std::map<std::string, std::string>& params) override final;
    };

} // namespace cola
