module;
#include <expected>
#include <filesystem>
#include <format>
#include <vector>
#include <zip.h>
export module pharmaco.core.zip:builder;
import :error;
import :cleaner;

namespace pharmaco::core {
  namespace fs = std::filesystem;
  export class zip_builder {
    fs::path __path;
    fs::path __root;
    std::vector<std::string> __added;
    std::unique_ptr<zip_t, zip_discarder> __zip;
    zip_builder(fs::path path, fs::path root, std::unique_ptr<zip_t, zip_discarder> zip) :
      __path{std::move(path)}, __root{root}, __zip{std::move(zip)} {}

  public:
    std::expected<fs::path, zip_error> make_zip_path(const fs::path &p) const {
      auto rel_p = fs::proximate(p, __root);
      if (std::string_view{rel_p.c_str()}.starts_with("..")) {
        return std::unexpected{
          zip_error(std::format("{} is not a child of {}", p.c_str(), __root.c_str()))
        };
      } else {
        return rel_p;
      }
    }
    std::expected<std::reference_wrapper<zip_builder>, zip_error> add_zip_source(
      std::string_view rel_path, std::unique_ptr<zip_source_t, zip_source_closer> buf
    ) {
      auto index =
        zip_file_add(__zip.get(), rel_path.data(), buf.get(), ZIP_FL_OVERWRITE | ZIP_FL_ENC_UTF_8);
      if (index == -1) {
        return std::unexpected{zip_error{zip_get_error(__zip.get())}};
      }
      return std::ref(*this);
    }
    std::expected<std::reference_wrapper<zip_builder>, zip_error> add_file(const fs::path &p) {
      using expected_type = std::expected<std::reference_wrapper<zip_builder>, zip_error>;
      return make_zip_path(p).and_then([&](auto &&rel_path) {
        zip_source_t *buf = zip_source_file(__zip.get(), p.c_str(), 0, ZIP_LENGTH_TO_END);
        if (buf == nullptr) {
          return expected_type{std::unexpected{zip_error{zip_get_error(__zip.get())}}};
        } else {
          return add_zip_source(
            rel_path.c_str(),
            std::unique_ptr<zip_source_t, zip_source_closer>{buf, zip_source_closer{}}
          );
        }
      });
    }
    std::expected<std::reference_wrapper<zip_builder>, zip_error> add_file(
      std::string_view rel_path, std::string_view content
    ) {
      zip_source_t *buf = zip_source_buffer(
        __zip.get(), static_cast<const void *>(content.data()), content.size(), 0
      );
      if (buf == nullptr) {
        return std::unexpected{zip_error{zip_get_error(__zip.get())}};
      } else {
        return add_zip_source(
          rel_path, std::unique_ptr<zip_source_t, zip_source_closer>{buf, zip_source_closer{}}
        );
      }
    }
    std::expected<std::reference_wrapper<zip_builder>, zip_error> add_directory(const fs::path &p) {
      using expected_type = std::expected<std::reference_wrapper<zip_builder>, zip_error>;
      return make_zip_path(p).and_then([&](auto &&rel_path) {
        auto index = zip_dir_add(__zip.get(), rel_path.c_str(), ZIP_FL_ENC_UTF_8);
        if (index == -1) {
          return expected_type{std::unexpected{zip_error{zip_get_error(__zip.get())}}};
        }
        for (const auto &entry : fs::directory_iterator(p)) {
          auto res = add_to_zip(entry.path());
          if (!res) {
            return expected_type{std::unexpected{res.error()}};
          }
        }
        return expected_type{std::ref(*this)};
      });
    }
    std::expected<std::reference_wrapper<zip_builder>, zip_error> add_to_zip(const fs::path &p) {
      if (fs::is_directory(p)) {
        return add_directory(p);
      } else {
        return add_file(p);
      }
    }
    std::expected<void, zip_error> finalize() {
      int err_no = zip_close(__zip.release());
      if (err_no == -1) {
        return std::unexpected{zip_error{zip_get_error(__zip.get())}};
      }
      return {};
    }

    static std::expected<zip_builder, zip_error> open(const fs::path &p, const fs::path &zip_root) {
      int err_code = 0;
      zip_t *zip_file = zip_open(p.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &err_code);
      if (zip_file == nullptr) {
        return std::unexpected{zip_error{err_code}};
      }
      auto zip_ptr = std::unique_ptr<zip_t, zip_discarder>{zip_file, zip_discarder{}};
      return zip_builder{p, fs::absolute(zip_root), std::move(zip_ptr)};
    }
  };
}