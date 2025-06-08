module;
#include <filesystem>
#include <string>
export module pharmaco.cli:config_identity;
import pharmaco.core.curl;

namespace pharmaco::cli {
  namespace fs = std::filesystem;
  struct config_identity {
    std::string name;
    std::string internal_name;
    std::string developer;
    std::vector<std::string> tags;

    fs::path get_icon_path(const fs::path &work_dir) const {
      return work_dir / "icons" / "icon.png";
    }
    fs::path get_dark_icon_path(const fs::path &work_dir) const {
      return work_dir / "icons" / "dark_icon.png";
    }
    fs::path get_docs_directory(const fs::path &work_dir) const {
      return work_dir / "docs";
    }
    fs::path get_py_module_path(const fs::path &work_dir) const {
      return work_dir / "manifest.py";
    }
    std::string py_module_name() const {
      return "manifest";
    }

    static config_identity make_default(
      std::string name, std::string internal_name, std::string developer
    ) {
      return config_identity{
        .name{std::move(name)},
        .internal_name{std::move(internal_name)},
        .developer{std::move(developer)},
        .tags{}
      };
    }
  };
}