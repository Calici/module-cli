module;
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <expected>
export module pharmaco.core.bindings.python:expected_caster;
import moderna.generic;
namespace generic = moderna::generic;
namespace PYBIND11_NAMESPACE {
  namespace detail {
    export template <class T, generic::is_formattable_error E>
    struct type_caster<std::expected<T, E>> {
    public:
      using expected_type = std::expected<T, E>;
      PYBIND11_TYPE_CASTER(expected_type, const_name("expected"));
      static handle cast(
        const std::expected<T, E> &src, return_value_policy policy, handle parent
      ) {
        if (src) {
          if constexpr (std::same_as<T, void>) {
            return none{}.release();
          } else {
            return type_caster<T>::cast(src.value(), policy, parent);
          }
        } else {
          return type_caster<E>::cast(src.error(), policy, parent);
        }
      }
    };
  }
}