
module;
#include <algorithm>
#include <atomic>
#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
export module pharmaco.core.runner:resources;

namespace pharmaco::core {
  export template <class data_type, std::invocable<data_type> deleter_type> class resource_guard {
    std::optional<data_type> __data;
    deleter_type __deleter;

  public:
    resource_guard(data_type data, deleter_type deleter) :
      __data{std::move(data)}, __deleter{std::move(deleter)} {}
    resource_guard(const resource_guard &guard) = delete;
    resource_guard(resource_guard &&guard) :
      __data{std::move(guard.__data)}, __deleter{std::move(guard.__deleter)} {
      guard.__data.reset();
    }
    resource_guard &operator=(const resource_guard &guard) = delete;
    resource_guard &operator=(resource_guard &&guard) {
      __data = std::move(guard.__data);
      __deleter = std::move(guard.__deleter);
      guard.__data.reset();
      return *this;
    }
    resource_guard(data_type data)
      requires(std::is_default_constructible_v<deleter_type>)
      : __data{std::move(data)}, __deleter{} {}
    bool has_value() {
      return __data.has_value();
    }
    const data_type &data() const noexcept {
      return __data.value();
    }
    void free() noexcept {
      if (__data) {
        __deleter(__data.value());
        __data.reset();
      }
    }
    ~resource_guard() {
      free();
    }
  };

  export struct gpu_resource_device_data {
    std::string gpu_id;
    size_t max_mem;
  };
  export class gpu_resource_device {
    gpu_resource_device_data __data;
    std::atomic<size_t> __used_mem;

  public:
    struct deleter {
      std::reference_wrapper<gpu_resource_device> device;
      void operator()(size_t mem) const noexcept {
        device.get().free(mem);
      }
    };
    gpu_resource_device(std::string gpu_id, size_t max_mem) :
      __data{std::move(gpu_id), max_mem}, __used_mem{0} {}
    gpu_resource_device(gpu_resource_device_data data) : __data{std::move(data)}, __used_mem{0} {}
    gpu_resource_device(const gpu_resource_device &device) : __data{device.__data}, __used_mem{0} {}

    std::string_view gpu_id() const {
      return __data.gpu_id;
    }
    size_t get_max_memory() const {
      return __data.max_mem;
    }
    size_t get_empty_memory() const {
      return __data.max_mem - get_used_memory();
    }
    size_t get_used_memory() const {
      return __used_mem.load(std::memory_order_relaxed);
    }

    /*
      allocate amount. can fail to allocate and a null.
    */
    std::optional<resource_guard<size_t, deleter>> try_allocate(size_t amount) {
      // Load current amount of used memory
      size_t current_mem = 0;
      size_t new_mem = current_mem + amount;
      if (new_mem > get_max_memory()) {
        return std::nullopt;
      }

      // Perform a CAS to swap the memory inside
      while (!__used_mem.compare_exchange_weak(
        current_mem, new_mem, std::memory_order_acq_rel, std::memory_order_acquire
      )) {
        new_mem = current_mem + amount;
        if (new_mem > get_max_memory()) {
          return std::nullopt;
        }
      }

      // This section is protected by the CAS above.
      return resource_guard{amount, deleter{*this}};
    }
    /*
      allocates as much as possible on the current device.
    */
    std::optional<resource_guard<size_t, deleter>> try_allocate_max() {
      size_t current_mem = 0;
      size_t new_mem = __data.max_mem;
      // CAS full memory
      while (!__used_mem.compare_exchange_weak(
        current_mem, __data.max_mem, std::memory_order_acq_rel, std::memory_order_relaxed
      )) {
      }
      if (current_mem == __data.max_mem) {
        return std::nullopt;
      } else {
        return resource_guard{__data.max_mem - current_mem, deleter{*this}};
      }
    }
    /*
      allocates amount or as much as possible less than amount.
    */
    std::optional<resource_guard<size_t, deleter>> alloc_amount_or_max(size_t amount) {
      size_t current_mem = 0;
      size_t new_mem = current_mem + amount;
      if (new_mem > get_max_memory()) {
        new_mem = get_max_memory();
      }
      /*
        CAS to the maximum amount of memory available or the max amount of memory.
      */
      while (!__used_mem.compare_exchange_weak(
        current_mem, new_mem, std::memory_order_acq_rel, std::memory_order_acquire
      )) {
        new_mem = current_mem + amount;
        if (new_mem > get_max_memory()) {
          new_mem = get_max_memory();
        }
      }
      // Get the amount of memory allocated
      amount = new_mem - current_mem;
      if (amount == 0) {
        return std::nullopt;
      } else {
        return resource_guard{amount, deleter{*this}};
      }
    }

    /*
      Free memory. Do not try to free memory that is not in this resource. Use the guard.
    */
    void free(size_t mem) {
      __used_mem.fetch_sub(mem, std::memory_order::acq_rel);
    }
  };

  export class gpu_resource_device_list {
    std::vector<gpu_resource_device> __devices;

  public:
    template <std::ranges::range device_range>
      requires(std::same_as<std::ranges::range_value_t<device_range>, gpu_resource_device_data>)
    gpu_resource_device_list(device_range devices) {
      for (const auto &device : devices) {
        __devices.emplace_back(device);
      }
    }
    template <class... Args>
      requires(std::same_as<gpu_resource_device_data, Args> && ...)
    gpu_resource_device_list(Args &&...devices) {
      (__devices.emplace_back(std::forward<Args>(devices)), ...);
    }

    std::optional<std::vector<resource_guard<size_t, gpu_resource_device::deleter>>> try_allocate(
      size_t amount
    ) {
      std::vector<resource_guard<size_t, gpu_resource_device::deleter>> allocated;
      size_t allocated_amount = 0;
      for (auto &device : __devices) {
        auto local_alloc_guard = device.alloc_amount_or_max(amount - allocated_amount);
        if (local_alloc_guard) {
          allocated_amount += local_alloc_guard->data();
          allocated.emplace_back(std::move(local_alloc_guard.value()));
          if (allocated_amount == amount) {
            break;
          }
        }
      }
      // return only if every piece of memory is allocated;
      if (allocated_amount == amount) {
        return std::move(allocated);
      } else {
        return std::nullopt;
      }
    }

    std::optional<std::reference_wrapper<const gpu_resource_device>> get_device(size_t id) const {
      if (id < __devices.size()) {
        return std::cref(__devices[id]);
      } else {
        return std::nullopt;
      }
    }
    std::optional<std::reference_wrapper<const gpu_resource_device>> get_device(
      std::string_view gpu_id
    ) const {
      auto it = std::ranges::find(__devices, gpu_id, &gpu_resource_device::gpu_id);
      if (it == __devices.end()) {
        return std::nullopt;
      } else {
        return std::cref(*it);
      }
    }
    std::optional<size_t> get_max_memory(size_t id) const {
      return get_device(id).transform(&gpu_resource_device::get_max_memory);
    }
    std::optional<size_t> get_used_memory(size_t id) const {
      return get_device(id).transform(&gpu_resource_device::get_used_memory);
    }
    std::optional<size_t> get_empty_memory(size_t id) const {
      return get_device(id).transform(&gpu_resource_device::get_empty_memory);
    }
    std::optional<size_t> get_max_memory(std::string_view id) const {
      return get_device(id).transform(&gpu_resource_device::get_max_memory);
    }
    std::optional<size_t> get_used_memory(std::string_view id) const {
      return get_device(id).transform(&gpu_resource_device::get_used_memory);
    }
    std::optional<size_t> get_empty_memory(std::string_view id) const {
      return get_device(id).transform(&gpu_resource_device::get_empty_memory);
    }
    size_t device_count() const {
      return __devices.size();
    }

    size_t get_max_memory() const {
      size_t max_memory = 0;
      for (const auto &device : __devices) {
        max_memory += device.get_max_memory();
      }
      return max_memory;
    }

    size_t get_used_memory() const {
      size_t used_memory = 0;
      for (const auto &device : __devices) {
        used_memory += device.get_used_memory();
      }
      return used_memory;
    }

    size_t get_empty_memory() const {
      size_t empty_memory = 0;
      for (const auto &device : __devices) {
        empty_memory += device.get_empty_memory();
      }
      return empty_memory;
    }
  };
}
