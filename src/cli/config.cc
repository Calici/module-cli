module;
#include <rfl/yaml.hpp>
#include <algorithm>
#include <expected>
#include <filesystem>
#include <functional>
#include <optional>
#include <rfl.hpp>
#include <string>
#include <vector>
export module pharmaco.cli:config;
import moderna.cli;
import moderna.process;

namespace fs = std::filesystem;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  /*
    version_config defines the version settings for each module version.
  */
  export struct version_config {
    cli::app_version version;
    std::string description;
    bool allow_use = true;
    bool is_beta = true;
    std::string git_tag;

    static void sort(std::vector<version_config> &version_configs, bool new_first = true) {
      if (new_first) {
        std::ranges::sort(
          version_configs, std::greater<cli::app_version>{}, &version_config::version
        );
      } else {
        std::ranges::sort(version_configs, std::less<cli::app_version>{}, &version_config::version);
      }
    }
  };

  /*
    container_config defines the configuration used to build and run the container.
  */
  export struct container_build_config {
    fs::path context;
    std::vector<std::string> build_args;

    static container_build_config make_default() {
      return container_build_config{.context{"."}, .build_args{}};
    }
  };
  export struct container_config {
    fs::path dockerfile;
    container_build_config build;

    std::expected<proc::subprocess_result, proc::subprocess_error> build_container(
      std::string_view container_name
    ) const {
      proc::subprocess_argument args{"docker", "build", "-t", container_name};
      args.add_argument(build.build_args);
      // Add the context
      args.add_argument(build.context.c_str());
      return proc::run(args);
    }

    static container_config make_default() {
      return container_config{
        .dockerfile{"dockerfile"}, .build{container_build_config::make_default()}
      };
    }
  };

  /*
    test_config defines the configuration used to define a test.
  */
  export struct test_config {
    std::string name;
    fs::path test_data;
  };

  /*
    This defines the config.yml file used to create a module
  */
  export struct module_config {
    std::string name;
    std::string internal_name;
    std::string developer;
    fs::path manifest_file;
    fs::path docs;
    fs::path icon;
    fs::path dark_icon;
    container_config container;
    std::vector<test_config> tests;
    std::vector<version_config> versions;

    static module_config make_default(
      std::string name, std::string internal_name, std::string developer
    ) {
      return module_config{
        .name{std::move(name)},
        .internal_name{std::move(internal_name)},
        .developer{std::move(developer)},
        .manifest_file{"manifest.py"},
        .docs{"docs"},
        .icon{"icon/icon.png"},
        .dark_icon{"icon/dark_icon.png"},
        .container{container_config::make_default()},
        .tests{},
        .versions{version_config{
          .version{0, 0, 0},
          .description{"First Release"},
          .allow_use = true,
          .is_beta = true,
          .git_tag{"v0.0.0"}
        }}
      };
    }
  };

  export class module_config_file {
  private:
    fs::path __p;
    fs::path __dir;
    module_config __config;
    module_config_file(fs::path p, module_config conf) :
      __p{std::move(p)}, __dir{__p.parent_path()}, __config{std::move(conf)} {}

  public:
    const module_config &config() const noexcept {
      return __config;
    }
    const fs::path &path() const noexcept {
      return __p;
    }
    const fs::path &dir() const noexcept {
      return __dir;
    }
    fs::path manifest_file() const noexcept {
      return dir() / __config.manifest_file;
    }
    fs::path docs() const noexcept {
      return dir() / __config.docs;
    }
    fs::path icon() const noexcept {
      return dir() / __config.icon;
    }
    fs::path dark_icon() const noexcept {
      return dir() / __config.dark_icon;
    }
    fs::path dockerfile() const noexcept {
      return dir() / __config.container.dockerfile;
    }
    /*
      Returns the module name
    */
    std::string_view name() const noexcept {
      return __config.name;
    }
    std::string_view internal_name() const noexcept {
      return __config.internal_name;
    }
    /*
      Returns the container name for the most recent version
    */
    std::string container_name() const {
      auto max_version =
        std::ranges::max_element(__config.versions, std::less{}, &version_config::version);
      return std::format("{}:{}", __config.internal_name, max_version->version);
    }
    /*
      Returns the container name for a given version. Returns an empty optional if the version
      does not exist
    */
    std::optional<std::string> container_name(const cli::app_version &version) const {
      return get_version(version).transform([&](const version_config &config) {
        return std::format("{}:{}", __config.internal_name, config.version);
      });
    }
    /*
      gets a version and returns an empty optional if the version does not exist
    */
    std::optional<std::reference_wrapper<const version_config>> get_version(
      const cli::app_version &v
    ) const noexcept {
      auto version = std::ranges::find(__config.versions, v, &version_config::version);
      if (version == __config.versions.end()) return std::nullopt;
      return std::cref(*version);
    }
    std::optional<std::reference_wrapper<const version_config>> get_version(
      size_t id
    ) const noexcept {
      if (id < versions().size()) {
        return std::cref(__config.versions[id]);
      } else {
        return std::nullopt;
      }
    }
    std::optional<std::reference_wrapper<const version_config>> set_version(
      const version_config &v
    ) noexcept {
      auto version = std::ranges::find(__config.versions, v.version, &version_config::version);
      if (version == __config.versions.end()) return std::nullopt;
      *version = v;
      return std::cref(*version);
    }
    /*
      adds a version, returns an empty optional if the given version already exists.
    */
    std::optional<std::reference_wrapper<const version_config>> add_version(
      const cli::app_version &new_version,
      std::string_view description,
      bool is_beta = true,
      std::optional<std::string_view> git_tag = std::nullopt
    ) {
      auto version = get_version(new_version);
      if (version) return std::nullopt;
      auto &added_version = __config.versions.emplace_back(
        version_config{
          .version{new_version},
          .description{description},
          .allow_use = true,
          .is_beta = is_beta,
          .git_tag = git_tag.transform([](std::string_view v) { return std::string{v}; }
          ).value_or(std::format("v{}", new_version))
        }
      );
      return std::cref(added_version);
    }
    /*
      gets the version size
    */
    size_t version_size() const noexcept {
      return config().versions.size();
    }
    /*
      For version iteration
    */
    const std::vector<version_config> &versions() const noexcept {
      return config().versions;
    }
    /*
      gets a test given the name, returns an empty optional when the test does not exist
    */
    std::optional<std::reference_wrapper<const test_config>> get_test(
      std::string_view test_name
    ) const {
      auto test = std::ranges::find(__config.tests, test_name, &test_config::name);
      if (test == __config.tests.end()) return std::nullopt;
      return std::cref(*test);
    }
    /*
      adds a test into the test suite, returns a null if the test is not added
    */
    std::optional<std::reference_wrapper<const test_config>> add_test(
      std::string_view test_name, const fs::path &test_data
    ) {
      auto test = get_test(test_name);
      if (test) {
        return std::nullopt;
      }
      auto &added_config =
        __config.tests.emplace_back(test_config{std::string{test_name}, std::move(test_data)});
      return std::cref(added_config);
    }
    /*
      Validates the configuration file. This asserts that the file have at least one version for
      now.
    */
    std::expected<void, rfl::Error> validate_version() {
      if (version_size() == 0)
        return std::unexpected{rfl::Error{"Please manually add one version to the config file"}};
      else
        return {};
    }
    /*
      Removes a test from a test suite. returns the removed test if the test exists and returns
      an empty optional if the test does not exist
    */
    std::optional<test_config> remove_test(std::string_view test_name) {
      auto test = std::ranges::find(__config.tests, test_name, &test_config::name);
      if (test == __config.tests.end()) return std::nullopt;
      test_config test_to_delete = std::move(*test);
      __config.tests.erase(test);
      return test_to_delete;
    }
    /*
      saves the current file.
    */
    std::expected<void, rfl::Error> save() const {
      return rfl::yaml::save(__p, __config).transform([&](auto &&) -> void { return; });
    }

    /*
      loads the fileƒ
    */
    static std::expected<module_config_file, rfl::Error> load(fs::path p) {
      return rfl::yaml::load<module_config>(p).and_then([&](auto &&conf) {
        auto file = module_config_file{std::move(p), std::move(conf)};
        return file.validate_version().transform([&]() { return std::move(file); });
      });
    }

    /*
      Loads a new file.
    */
    static std::expected<module_config_file, rfl::Error> new_file(
      fs::path p, std::string name, std::string internal_name, std::string developer
    ) {
      auto conf = module_config_file{
        std::move(p),
        module_config::make_default(std::move(name), std::move(internal_name), std::move(developer))
      };
      return conf.save().transform([&]() { return std::move(conf); });
    }
  };
}

namespace ph = pharmaco::cli;
template <class char_type> struct std::formatter<ph::version_config, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const ph::version_config &conf, auto &ctx) const {
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
