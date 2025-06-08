module;
#include <rfl/yaml.hpp>
#include <filesystem>
#include <format>
export module pharmaco.cli:sandbox_action;
import moderna.cli;
import pharmaco.core.curl;
import moderna.type_check;
import moderna.io;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace tc = moderna::type_check;
  namespace fs = std::filesystem;
  namespace io = moderna::io;
  struct type_tester_action {
    void operator()(cli::app &app) {
      app.add_argument("--type").help("The type to test");
      app.add_argument("-v").as_flag().help("Verbosely print if type is valid");
      app.parse_argument();
      auto type_arg = cli::app::test_expected(
        tc::parse_string(app.arguments().first_of("--type").value()), 1, "[x] '{}' is invalid:"
      );
      bool verbose = app.arguments().contains("-v");
      cli::app::out("[√] '{}' is a valid type\n", type_arg);
      if (verbose) {
        cli::app::out("Type Hierarchy Tree:\n{}", rfl::yaml::write(type_arg));
      }
    }
  };

  struct query_action {
    std::string make_request(std::string_view url) {
      return cli::app::test_expected(
        core::curl_http_request::make_request(url, core::curl_http_request_method::GET)
          .and_then([](core::curl_http_request &&req) { return req.send(); })
          .transform([](core::curl_http_response &&resp) { return std::move(resp.body); })
      );
    }
    void save_to_file(const fs::path &p, std::string_view content) {
      return cli::app::test_expected(
        io::open_file<io::open_mode::write_truncate>(p)
          .and_then([&](auto &&f) { return f.write(content); })
          .transform([](auto &&v) { return; })
      );
    }
    void operator()(cli::app &app) {
      app.add_argument("--url").help("The url to query for").required();
      app.add_argument("--output").help("The file to output to").required();
      app.parse_argument();
      auto url = app.arguments()
                   .first_of("--url")
                   .transform([](cli::argument_value &&value) {
                     return value.cast<std::string_view>().value();
                   })
                   .value();
      auto output =
        app.arguments()
          .first_of("--output")
          .transform([](cli::argument_value &&value) { return value.cast<fs::path>().value(); })
          .value();
      auto resp = make_request(url);
      save_to_file(output, resp);
    }
  };
  export struct sandbox_action {
    void operator()(cli::app &app) {
      app.build_action("Use sandbox functions to test configurations")
        .add_action(
          "type_tester", "Test if a string can be parsed into a pharmaco type", type_tester_action{}
        )
        .add_action("query", "Test a GET query to a specific url", query_action{})
        .run();
    }
  };
}