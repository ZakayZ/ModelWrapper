#include <FermiBreakUp/FermiBreakUp.h>
#include <iostream>

#include "wrapper.h"

using namespace wrapper;

int main() {
    auto model = FermiWrapper(std::unique_ptr<fermi::VFermiBreakUp>(new fermi::FermiBreakUp()));

    auto ptr = config_builder::ModelRegistry::Find("fermi");
    std::cout << ptr.has_value() << '\n';

    std::cout << "adasddas" << std::endl;
}
