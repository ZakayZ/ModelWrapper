#pragma once

namespace config_builder {

    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    namespace detail {
        template <typename F>
        class Deferer {
        public:
            Deferer(F&& func) : func_(std::move(func)) {}

            Deferer(const Deferer&) = delete;
            Deferer(Deferer&&) = delete;

            Deferer& operator=(const Deferer&) = delete;
            Deferer& operator=(Deferer&&) = delete;

            ~Deferer() {
                func_();
            }

        private:
            F func_;
        };

        struct DeferHelper {
            template <typename F>
            Deferer<F> operator<<(F&& func) {
                return Deferer<F>(std::forward<F>(func));
            }
        };

    }  // namespace detail

    #define TOKENPASTE(x, y) x ## y
    #define TOKENPASTE2(x, y) TOKENPASTE(x, y)
    #define DEFER auto __deffer_lambda##__COUNTER__ = detail::DeferHelper{} << [&]() -> void

    #undef TOKENPASTE
    #undef TOKENPASTE2

} // namespace config_builder
