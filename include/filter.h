#pragma once

#include <functional>
#include <memory>

#include <COLA.hh>
#include "lib/config/module.h"

namespace cola {

    class ConfigConverter final: public cola::VConverter {
    public:
        ConfigConverter(std::unique_ptr<config_builder::Module>&& model);

        std::unique_ptr<cola::EventData> operator()(std::unique_ptr<cola::EventData>&& event) override final;

    private:
        std::unique_ptr<config_builder::Module> model_;
    };

}
