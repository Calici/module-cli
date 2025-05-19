module;
#include <concepts>
#include <functional>
#include <optional>
#include <vector>
export module pharmaco.core:resource;

namespace pharmaco::core {
  template <class alloc_type, std::invocable<alloc_type> deleter_type> class resource_guard {
    std::optional<alloc_type> __alloc;
    deleter_type __deleter;

    void __free_current() {
      if (__alloc.has_value()) {
        std::invoke(__deleter, std::move(__alloc).value());
      }
    }

  public:
    resource_guard(alloc_type alloc, deleter_type deleter) :
      __alloc{std::move(alloc)}, __deleter{std::move(deleter)} {}

    resource_guard(const resource_guard &) = delete;
    resource_guard &operator=(const resource_guard &other) = delete;

    resource_guard(resource_guard &&other) :
      __alloc{std::move(other.__alloc)}, __deleter{std::move(other.__deleter)} {}
    resource_guard &operator=(resource_guard &&other) {
      __free_current();
      __alloc = std::move(other.__alloc);
      __deleter = std::move(other.__deleter);
      return *this;
    }

    bool has_value() const {
      return __alloc.has_value();
    }

    ~resource_guard() {
      __free_current();
    }
  };

  class gpu_memory {
    size_t __max_mem;
    size_t __used_mem;

  public:
    struct gpu_memory_deleter {
      std::reference_wrapper<gpu_memory> ref;

      void operator()(size_t amount) const noexcept {
        ref.get().__used_mem -= amount;
      }
    };

    gpu_memory(size_t max_mem, size_t used_mem = 0) : __max_mem{max_mem}, __used_mem{used_mem} {}

    size_t max_mem() const {
      return __max_mem;
    }

    bool is_usable(size_t use_amount) const {
      return use_amount + __used_mem <= __max_mem;
    }

    size_t free_mem() const {
      return __max_mem - __used_mem;
    }

    std::optional<resource_guard<size_t, gpu_memory_deleter>> use(size_t use_amount) {
      if (is_usable(use_amount)) {
        __used_mem += use_amount;
        return resource_guard<size_t, gpu_memory_deleter>{
          use_amount, gpu_memory_deleter{std::ref(*this)}
        };
      }
      return std::nullopt;
    }

    size_t release(resource_guard<size_t, gpu_memory_deleter> resource) {
      // auto release
      return free_mem();
    }
  };
}