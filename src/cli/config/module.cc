module;
#include <rfl/yaml.hpp>
#include <filesystem>
#include <format>
#include <optional>
#include <rfl.hpp>
#include <string>
#include <string_view>
export module pharmaco.cli:config_module;
import moderna.cli;
import moderna.process;
import :config_container;
import :config_identity;
import :config_test;
import :config_version;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace fs = std::filesystem;
  namespace proc = moderna::process;
  struct config_module {
    config_identity identity;
    config_container container;
    config_test_list tests;
    config_version_list versions;

    /*
      File Contents
    */
    fs::path get_dockerfile_path(const fs::path &work_dir) const {
      return container.get_dockerfile_path(work_dir);
    }
    fs::path get_py_module_path(const fs::path &work_dir) const {
      return identity.get_py_module_path(work_dir);
    }
    fs::path get_icon_path(const fs::path &work_dir) const {
      return identity.get_icon_path(work_dir);
    }
    fs::path get_dark_icon_path(const fs::path &work_dir) const {
      return identity.get_dark_icon_path(work_dir);
    }
    fs::path get_docs_directory(const fs::path &work_dir) const {
      return identity.get_docs_directory(work_dir);
    }
    std::string py_module_name() const {
      return identity.py_module_name();
    }

    /*
      Version Control
    */
    std::optional<std::reference_wrapper<const config_version>> get_version(
      const cli::app_version &v
    ) const noexcept {
      return versions.get(v);
    }
    const config_version &set_version(const config_version &v) {
      return versions.set(v);
    }
    std::optional<config_version> remove_version(const cli::app_version &v) {
      return versions.remove(v);
    }

    /*
      Test Control
    */
    std::optional<std::reference_wrapper<const config_test>> get_test(
      std::string_view test_name
    ) const noexcept {
      return tests.get(test_name);
    }
    const config_test &set_test(const config_test &test) {
      return tests.set(test);
    }
    std::optional<config_test> remove_test(std::string_view test_name) {
      return tests.remove(test_name);
    }

    /*
      File Control
    */
    std::expected<void, rfl::Error> save(const fs::path &path) const {
      return rfl::yaml::save(path.c_str(), *this).transform([](auto &&) { return; });
    }

    /*
      Container Control
    */
    std::string get_image_name() const {
      return std::format("{}:{}", identity.internal_name, versions.get_newest().get().version);
    }
    std::optional<std::string> get_image_name(const cli::app_version &v) const {
      return get_version(v).transform([&](auto &&) {
        return std::format("{}:{}", identity.internal_name, v);
      });
    }

    proc::subprocess_argument get_build_arguments(
      std::string_view container_name, const fs::path &work_dir
    ) const {
      return container.get_build_arguments(container_name, work_dir);
    }

    static config_module make_default(
      std::string name, std::string internal_name, std::string developer
    ) {
      return config_module{
        .identity{config_identity::make_default(
          std::move(name), std::move(internal_name), std::move(developer)
        )},
        .container{config_container::make_default()},
        .tests{config_test_list::make_default()},
        .versions{config_version_list::make_default()}
      };
    }

    static std::expected<config_module, rfl::Error> load_config(const fs::path &path) {
      return rfl::yaml::load<config_module>(path.c_str());
    }
  };
}