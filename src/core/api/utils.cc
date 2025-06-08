module;
#include <optional>
#include <vector>
export module pharmaco.core.api:utils;

namespace pharmaco::core::api {
  struct entry_coercer {
    template <class T> std::optional<T> operator()(std::vector<T> data) const {
      std::optional<T> res = std::nullopt;
      if (data.empty()) return res;
      res = std::move(data[0]);
      return res;
    }
  };
  
}