//
#include "Utilities/DefineTypes.h"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"
//
#include <functional>
#include <tuple>

namespace elastica {

  namespace forcing {

    namespace activation {

      template <typename T>
      real_t identity_element(std::multiplies<T>) noexcept {
        return real_t(1.0);
      }

      template <typename T>
      real_t identity_element(std::plus<T>) noexcept {
        return real_t(0.0);
      }

      template <typename Op, typename... F>
      struct Compose {
        using type = std::tuple<F...>;

        explicit Compose(F... f) : data(std::move(f)...) {}

        auto operator()(real_t s, real_t t) const {
          using ::elastica::forcing::activation::identity_element;
          return cpp17::apply(
              [=](F const&... funcs) {
                auto v = identity_element(op);
                EXPAND_PACK_LEFT_TO_RIGHT((v = op(v, funcs(s, t))));
                return v;
              },
              data);
        }

        Op op;
        type data;
      };

      template <typename... Acts>
      decltype(auto) multiply(Acts... activations) {
        using RT = Compose<std::multiplies<void>, Acts...>;
        return RT(std::move(activations)...);
      }

      template <typename... Acts>
      decltype(auto) add(Acts... activations) {
        using RT = Compose<std::plus<void>, Acts...>;
        return RT(std::move(activations)...);
      }

    }  // namespace activation

  }  // namespace forcing

}  // namespace elastica