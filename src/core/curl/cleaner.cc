module;
#include <curl/curl.h>
#include <memory>
export module pharmaco.core.curl:cleaner;

namespace pharmaco::core {
  export struct curl_easy_cleaner {
    void operator()(CURL *curl) {
      if (curl != nullptr) {
        curl_easy_cleanup(curl);
      }
    }
  };

  export struct curl_slist_cleaner {
    void operator()(struct curl_slist *list) {
      if (list != nullptr) {
        curl_slist_free_all(list);
      }
    }
  };

  export using curl_pointer = std::unique_ptr<CURL, curl_easy_cleaner>;
  export using curl_list_pointer = std::unique_ptr<struct curl_slist, curl_slist_cleaner>;
}