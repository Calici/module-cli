module;
#include <algorithm>
#include <filesystem>
#include <rfl.hpp>
#include <string>
#include <string_view>
#include <vector>
export module pharmaco.cli:config_test;

namespace fs = std::filesystem;
namespace pharmaco::cli {
  struct config_test {
    std::string name;
    fs::path run_yml;
  };
  class config_test_list {
    std::vector<config_test> __tests;

  public:
    config_test_list() : __tests{} {}
    config_test_list(size_t pre_allocate_size) {
      __tests.reserve(pre_allocate_size);
    }
    const config_test &set(const config_test &test) {
      auto q_test = std::ranges::find(__tests, test.name, &config_test::name);
      if (q_test == __tests.end()) {
        auto &added_test = __tests.emplace_back(test);
        return added_test;
      } else {
        *q_test = test;
        return *q_test;
      }
    }
    std::optional<std::reference_wrapper<const config_test>> get(
      std::string_view test_name
    ) const noexcept {
      auto q_test = std::ranges::find(__tests, test_name, &config_test::name);
      if (q_test == __tests.end()) {
        return std::nullopt;
      } else {
        return std::cref(*q_test);
      }
    }
    std::optional<config_test> remove(std::string_view test_name) {
      auto test = std::ranges::find(__tests, test_name, &config_test::name);
      if (test == __tests.end()){
        return std::nullopt;
      }
      else {
        config_test removed_test = std::move(*test);
        __tests.erase(test);
        return std::move(removed_test);
      }
    }
    auto begin() const noexcept {
      return __tests.begin();
    }
    auto end() const noexcept {
      return __tests.end();
    }
    size_t size() const noexcept {
      return __tests.size();
    }

    static config_test_list make_default() {
      return config_test_list{};
    }
  };
}

template <> struct rfl::Reflector<pharmaco::cli::config_test_list> {
  using ReflType = std::vector<pharmaco::cli::config_test>;
  static pharmaco::cli::config_test_list to(const ReflType &raw_tests) {
    pharmaco::cli::config_test_list tests;
    for (const auto &raw_test : raw_tests) {
      tests.set(raw_test);
    }
    return tests;
  }
  static ReflType from(const pharmaco::cli::config_test_list &tests) {
    ReflType raw_tests;
    for (const auto &test : tests) {
      raw_tests.emplace_back(test);
    }
    return raw_tests;
  }
};