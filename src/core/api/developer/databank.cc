module;
#include <string>
#include <rfl.hpp>
export module pharmaco.core.api:databank;

namespace pharmaco::core::api {
  struct databank_data {
    std::string name;
    std::string internal_name;
    std::string uuid;

    rfl::ExtraFields<rfl::Generic> extras;
  };
}