module;
#include <rfl/yaml.hpp>
#include <filesystem>
#include <format>
#include <regex>
#include <string_view>
export module pharmaco.cli:docs_action;
import moderna.cli;
import moderna.io;
import moderna.process;
import :loader;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  namespace io = moderna::io;
  namespace fs = std::filesystem;
  struct docs_run_action {
    fs::path assets_dir;
    fs::path get_temp_docs_config_path() const {
      return fs::current_path() / ".tmp" / "mkdocs.yml";
    }
    auto write_temp_docs_config(std::string_view docs_config) {
      auto config_path = get_temp_docs_config_path();
      fs::create_directories(config_path.parent_path());
      return cli::app::test_expected(
        io::open_file<io::open_mode::write_truncate>(config_path).and_then([&](auto &&file) {
          return file.write(docs_config);
        })
      );
    }
    /*
      Read the temporary documentation config file.
    */
    auto read_docs_config(
      std::string_view module_name, std::string_view addr, std::string_view docs_dir
    ) {
      return cli::app::test_expected(
        io::open_file<io::open_mode::read>(assets_dir / "mkdocs.yml").and_then([&](auto &&config) {
          return config.read().transform([&](auto &&res) {
            res = std::regex_replace(res, std::regex("\\$MODULE_NAME"), module_name.data());
            res = std::regex_replace(res, std::regex("\\$DEV_ADDR"), addr.data());
            res = std::regex_replace(res, std::regex("\\$DOCS_DIR"), docs_dir.data());
            return res;
          });
        })
      );
    }
    auto mkdocs_check() {
      return cli::app::test_expected(proc::run({"which", "mkdocs"}));
    }
    auto mkdocs_run(std::string_view docs_config) {
      return cli::app::test_expected(
        proc::run(
          proc::subprocess_argument{
            "mkdocs", "serve", "--config-file", get_temp_docs_config_path().c_str()
          },
          true
        )
      );
    }
    int get_serve_port(cli::app &app) {
      return cli::app::test_expected(app.arguments()
                                       .first_of("--port")
                                       .transform([&](auto &&v) { return v.template cast<int>(); })
                                       .value_or(3000));
    }
    void operator()(cli::app &app) {
      app.add_argument("--port").help("The port to run the documentations at. Defaults to 3000");
      app.parse_argument();
      int port = get_serve_port(app);
      auto module_dir = fs::current_path();
      auto config = load_config_module();
      auto dev_addr = std::format("localhost:{}", port);
      auto docs_dir = config.get_docs_directory(module_dir);
      auto docs_config = read_docs_config(config.identity.name, dev_addr, docs_dir.c_str());
      write_temp_docs_config(docs_config);
      mkdocs_check();
      mkdocs_run(docs_config);
    }
  };
  export struct docs_action {
    fs::path assets_dir;
    void operator()(cli::app &app) {
      app.build_action("Check documentation and Preview documentations live !")
        .add_action("run", "Run the documentation live in localhost", docs_run_action{assets_dir})
        .run();
    }
  };
}