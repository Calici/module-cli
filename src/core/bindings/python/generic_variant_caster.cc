module;
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <variant>
export module pharmaco.core.bindings.python:generic_variant_caster;
import moderna.generic;

namespace generic = moderna::generic;
namespace PYBIND11_NAMESPACE {
  namespace detail {
    template <class... Args>
    struct type_caster<generic::variant<Args...>> : public type_caster<std::variant<Args...>> {
      PYBIND11_TYPE_CASTER(generic::variant<Args...>, const_name("GenericVariant"));
      using super = type_caster<std::variant<Args...>>;
      bool load(handle src, bool p) {
        bool is_loaded = super::load(src, p);
        if (is_loaded) {
          value.value = std::move(super::value);
        }
        return is_loaded;
      }
      static handle cast(
        const generic::variant<Args...> &v, return_value_policy policy, handle parent
      ) {
        return super::cast(v.value, policy, parent);
      }
    };
  }
}