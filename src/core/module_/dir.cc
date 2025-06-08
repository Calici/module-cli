module;
#include <rfl/yaml.hpp>
#include <filesystem>
#include <optional>
#include <rfl.hpp>
export module pharmaco.core.module_:dir;

namespace pharmaco::core {
  namespace fs = std::filesystem;
  /*
      Encapsulates the value derivation for a module directory. Essentially, this encapsulates the
      module directory structure.
    */
  export class module_dir {
    fs::path __dir;
    std::optional<fs::path> __stdout_path;
    std::optional<fs::path> __stderr_path;
    module_dir(
      fs::path dir,
      std::optional<fs::path> stdout_path = std::nullopt,
      std::optional<fs::path> stderr_path = std::nullopt
    ) :
      __dir{std::move(dir)}, __stdout_path{std::move(stdout_path)},
      __stderr_path(std::move(stderr_path)) {
      /* dir is guaranteed to be a directory */
      create_directories();
    }

  public:
    const fs::path &dir() const {
      return __dir;
    }
    /*
      This returns the path to the module running directory.
    */
    fs::path run_path() const {
      return __dir / "run";
    }
    /*
      This returns the path to the module dependencies directory from the previous module
    */
    fs::path depends_path() const {
      return __dir / "depends";
    }
    /*
      This returns the path to the module logs path
    */
    fs::path logs_path() const {
      return __dir / "logs";
    }
    fs::path stdout_path() const {
      return __stdout_path.or_else([&]() { return std::optional{logs_path() / "stdout.log"}; }
      ).value();
    }
    fs::path stderr_path() const {
      return __stderr_path.or_else([&]() { return std::optional{logs_path() / "stderr.log"}; }
      ).value();
    }
    /*
      Params path
    */
    fs::path params_path() const {
      return run_path() / "params.yml";
    }

    std::expected<void, rfl::Error> write_module_parameters(
      const rfl::Generic::Object &parameters
    ) const {
      return rfl::yaml::save(params_path().string(), parameters).transform([](auto &&) -> void {
        return;
      });
    }
    void clear_content() const {
      fs::remove_all(run_path());
      fs::remove_all(depends_path());
      fs::remove_all(logs_path());
      create_directories();
    }
    void create_directories() const {
      fs::create_directories(run_path());
      fs::create_directories(depends_path());
      fs::create_directories(logs_path());
    }
    /*
      Creates the directory at path. If dir is a file, the creation will fail and std::nullopt
      will be returned.
    */
    static std::optional<module_dir> make_dir(const fs::path &dir) {
      if (fs::exists(dir) && !fs::is_directory(dir)) {
        return std::nullopt;
      } else {
        if (!fs::exists(dir)) fs::create_directories(dir);
        return module_dir{std::move(dir)};
      }
    }
  };
}