module;
#include <algorithm>
#include <format>
#include <functional>
#include <rfl.hpp>
#include <vector>
export module pharmaco.cli:config_version;
import moderna.cli;

namespace pharmaco::cli {
  namespace cli = moderna::cli;

  struct config_version {
    cli::app_version version;
    std::string description;
    bool allow_use = true;
    bool is_beta = true;
    std::string git_tag;
  };

  class config_version_list {
    std::vector<config_version> __versions;

  public:
    config_version_list() {}
    config_version_list(size_t pre_allocate_size) {
      __versions.reserve(pre_allocate_size);
    }

    const config_version &set(const config_version &v) {
      auto version = std::ranges::find(__versions, v.version, &config_version::version);
      if (version == __versions.end()) {
        auto &added_version = __versions.emplace_back(v);
        return std::cref(added_version);
      } else {
        *version = v;
        return std::cref(*version);
      }
    }
    std::optional<std::reference_wrapper<const config_version>> get(
      const cli::app_version &v
    ) const {
      auto version = std::ranges::find(__versions, v, &config_version::version);
      if (version == __versions.end()) {
        return std::nullopt;
      } else {
        return std::cref(*version);
      }
    }
    std::optional<config_version> remove(const cli::app_version &v) {
      auto version = std::ranges::find(__versions, v, &config_version::version);
      if (version == __versions.end()) {
        return std::nullopt;
      } else {
        config_version removed_version = std::move(*version);
        __versions.erase(version);
        return std::move(removed_version);
      }
    }
    std::reference_wrapper<const config_version> get_newest() const {
      // At best this will be O(1)
      return std::cref(*std::ranges::max_element(
        __versions,
        [](const cli::app_version &l, const cli::app_version &r) { return l < r; },
        &config_version::version
      ));
    }
    auto begin() const noexcept {
      return __versions.begin();
    }
    auto end() const noexcept {
      return __versions.end();
    }
    size_t size() const noexcept {
      return __versions.size();
    }
    void sort() noexcept {
      std::ranges::sort(__versions, std::greater<cli::app_version>{}, &config_version::version);
    }
    config_version_list sort() const {
      config_version_list new_list = *this;
      new_list.sort();
      return new_list;
    }

    static config_version_list make_default() {
      config_version_list versions;
      versions.set(
        {.version{0, 0, 0},
         .description{"Initial Release"},
         .allow_use = true,
         .is_beta = true,
         .git_tag{"v0.0.0"}}
      );
      return versions;
    }
  };
}

template <> struct rfl::Reflector<pharmaco::cli::config_version_list> {
  using ReflType = std::vector<pharmaco::cli::config_version>;
  static pharmaco::cli::config_version_list to(const ReflType &raw_versions) {
    pharmaco::cli::config_version_list versions;
    for (const auto &raw_version : raw_versions) {
      versions.set(raw_version);
    }
    versions.sort();
    return versions;
  }
  static ReflType from(const pharmaco::cli::config_version_list &versions) {
    auto sorted_versions = versions.sort();
    ReflType raw_versions;
    for (const auto &version : sorted_versions) {
      raw_versions.emplace_back(version);
    }
    return raw_versions;
  }
};

namespace cli = pharmaco::cli;

template <class char_type> struct std::formatter<cli::config_version, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const cli::config_version &conf, auto &ctx) const {
    return std::format_to(
      ctx.out(),
      "{}: {} [{}] [{}] [{}]",
      conf.version,
      conf.description,
      conf.git_tag,
      conf.is_beta ? "beta" : "norm",
      conf.allow_use ? "✔" : "x"
    );
  }
};