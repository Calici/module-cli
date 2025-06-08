module;
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <expected>
export module pharmaco.core.bindings.python:expected_caster;
import moderna.generic;
namespace generic = moderna::generic;
namespace PYBIND11_NAMESPACE {
  namespace detail {
    template <class T, generic::is_formattable_error E>
    struct type_caster<std::expected<T, E>> : public type_caster<T>, public type_caster<E> {
    public:
      using expected_type = std::expected<T, E>;
      using value_caster = type_caster<T>;
      using error_caster = type_caster<E>;
      PYBIND11_TYPE_CASTER(expected_type, const_name("expected"));
      static handle cast(
        const std::expected<T, E> &src, return_value_policy policy, handle parent
      ) {
        if (src) {
          if constexpr (std::same_as<T, void>) {
            return none{}.release();
          } else {
            return value_caster::cast(src.value(), policy, parent);
          }
        } else {
          throw std::runtime_error{std::format("{}", generic::error_formatter{src.error()})};
        }
      }
    };
  }
}