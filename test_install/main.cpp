#include <XMLConfigModule/config_module.h>
// #include <XMLConfigModule/lib/config/registry.h>

REGISTER_CONDITION(aboba, [](const cola::Particle&) -> bool {return true;});

int main() {
    auto res = config_builder::ConditionRegistry::Find("aboba");

    std::cout << res.has_value();
}
