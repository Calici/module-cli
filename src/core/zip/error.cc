module;
#include <exception>
#include <string>
#include <zip.h>
export module pharmaco.core.zip:error;

namespace pharmaco::core {
  export class zip_error : public std::exception {
    std::string __msg;

  public:
    zip_error(int err_code) {
      zip_error_t err;
      zip_error_init_with_code(&err, err_code);
      __msg = zip_error_strerror(&err);
      zip_error_fini(&err);
    }
    zip_error(zip_error_t *err) {
      __msg = zip_error_strerror(err);
    }
    zip_error(std::string msg) : __msg{std::move(msg)} {}
    const char *what() const noexcept {
      return __msg.c_str();
    }
  };
}