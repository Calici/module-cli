module;
#include <algorithm>
#include <expected>
#include <format>
#include <string>
#include <string_view>
export module pharmaco.core.io_nodes:io_nodes;
import moderna.generic;
import moderna.type_check;

namespace tc = moderna::type_check;
namespace generic = moderna::generic;
namespace pharmaco::core {
  export class io_nodes {
    generic::key_vector<std::string, tc::generic_type> __nodes;

  public:
    io_nodes() : __nodes{} {}
    auto get(std::string_view name) const noexcept {
      return __nodes.get(name);
    }
    io_nodes &insert(std::string name, tc::generic_type param) {
      __nodes.insert(std::move(name), std::move(param));
      return *this;
    }
    std::expected<std::reference_wrapper<io_nodes>, tc::parse_error_trace> insert(
      std::string_view name, std::string_view type
    ) {
      return tc::parse_string(type).transform([&](auto &&type) {
        __nodes.insert(std::string{name}, std::move(type));
        return std::ref(*this);
      });
    }
    size_t size() const noexcept {
      return __nodes.size();
    }
    auto begin() const noexcept {
      return __nodes.begin();
    }
    auto end() const noexcept {
      return __nodes.end();
    }
    static io_nodes make_empty() {
      return io_nodes{};
    }
  };
}

namespace core = pharmaco::core;

template <class char_type> struct std::formatter<core::io_nodes, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  auto format(const core::io_nodes &nodes, auto &ctx) const {
    if (nodes.size() == 0) {
      std::format_to(ctx.out(), "[]");
    }
    for (const auto &[name, type] : nodes) {
      std::format_to(ctx.out(), "{}: {}\n", name, type);
    }
    return ctx.out();
  }
};