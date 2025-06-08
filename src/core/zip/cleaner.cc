module;
#include <memory>
#include <zip.h>
export module pharmaco.core.zip:cleaner;

namespace pharmaco::core {
  export struct zip_discarder {
    void operator()(zip_t *zip) const {
      if (zip != nullptr) {
        zip_discard(zip);
      }
    }
  };

  export struct zip_source_closer {
    void operator()(zip_source_t *zip) const {
      if (zip == nullptr) {
        zip_source_close(zip);
      }
    }
  };

  export using zip_pointer = std::unique_ptr<zip_t, zip_discarder>;
  export using zip_source_pointer = std::unique_ptr<zip_source_t, zip_source_closer>;
}