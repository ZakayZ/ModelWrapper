#pragma once

#include <type_traits>

#include <COLA.hh>
#include <G4Fragment.hh>
#include <G4FragmentVector.hh>

template<typename T, typename = void>
struct has_break_it_up: std::false_type {};

template<typename T>
struct has_break_it_up<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().BreakItUp(std::declval<G4Fragment>())), G4FragmentVector*>>>: std::true_type {};

template<typename T>
constexpr bool has_break_it_up_v = has_break_it_up<T>::value;

namespace wrapper {
    G4Fragment ColaToG4Fragment(const cola::Particle& particle);

    cola::Particle G4FragmentToCola(const G4Fragment& fragment);

    double GetMass(const cola::Particle& particle);

    double GetExcitationEnergy(const cola::Particle& particle);
} // namespace wrapper
