module;
#include <algorithm>
#include <atomic>
#include <chrono>
#include <expected>
#include <print>
#include <thread>
#include <vector>
export module pharmaco.core.module_:pool;
import moderna.process;
import moderna.generic;

namespace pharmaco::core {
  namespace proc = moderna::process;
  namespace generic = moderna::generic;
  namespace chrono = std::chrono;

  namespace process_wrapper {
    template <class wrapper_type>
    concept is = requires(wrapper_type wrapper) {
      { wrapper.process() } -> std::same_as<proc::subprocess &>;
    } || std::same_as<wrapper_type, proc::subprocess>;

    template <is wrapper_type> proc::subprocess &get_process(wrapper_type &wrapper) {
      if constexpr (std::same_as<wrapper_type, proc::subprocess>) {
        return wrapper;
      } else {
        return wrapper.process();
      }
    }

    /*
      when stop() is called, the process has to be waited for even if fails to execute some of
      its command.
    */
    template <class wrapper_type>
    concept has_stop = requires(wrapper_type wrapper) {
      generic::is_formattable_error<typename wrapper_type::stop_error_type>;
      {
        wrapper.stop()
      } -> std::same_as<
        std::expected<proc::subprocess_result, typename wrapper_type::stop_error_type>>;
    };

    template <class wrapper_type>
    concept has_on_exit = requires(wrapper_type wrapper, proc::subprocess_result res) {
      { wrapper.on_exit(res) };
    };
  };

  template <class wrapper_type>
  concept is_process_wrapper = process_wrapper::is<wrapper_type>;

  template <class wrapper_type>
  concept process_wrapper_has_stop = process_wrapper::has_stop<wrapper_type>;

  template <class wrapper_type>
  concept process_wrapper_has_on_exit = process_wrapper::has_on_exit<wrapper_type>;

  export template <is_process_wrapper wrapper_type> class process_pool {
    struct pool_entry {
      size_t id;
      wrapper_type process;
    };
    struct pool_result {
      size_t id;
      wrapper_type process;
      proc::subprocess_result result;
    };
    std::atomic<size_t> __ptr;
    generic::thread_guard<std::vector<pool_entry>> __processes;
    std::atomic_flag __run_loop;
    chrono::milliseconds __loop_timer;
    std::thread __t;

    std::vector<pool_result> __wait_processes() {
      std::vector<pool_result> results =
        __processes.read_mut([](std::vector<pool_entry> &processes) {
          std::vector<pool_result> waited;
          std::vector<pool_entry> unwaited;
          for (pool_entry &entry : processes) {
            proc::subprocess &proc = process_wrapper::get_process(entry.process);
            auto wait_res = proc.wait_non_blocking(false);
            // Process is waited
            if (wait_res && wait_res->has_value()) {
              waited.emplace_back(
                pool_result{entry.id, std::move(entry.process), wait_res->value()}
              );
            }
            // Process no result
            else if (wait_res) {
              unwaited.emplace_back(std::move(entry));
            }
            // wait() syscall error
            else {
              // log and insert back
              std::println(
                stderr, "proc(pid: {}, id: {}): {}", proc.pid(), entry.id, wait_res.error().what()
              );
            }
          }
          processes = std::move(unwaited);
          return waited;
        });
      for (pool_result &result : results) {
        __cleanup_entry(result.process, result.result);
      }
      return results;
    }

    void __loop() {
      __run_loop.wait(false);
      while (__run_loop.test()) {
        __wait_processes();
      }
      __processes.read_mut([](std::vector<pool_entry> &processes) {
        for (auto &process : processes) {
          process_wrapper::get_process(process.process).kill_and_wait();
        }
      });
    }

    size_t __get_insert_id() {
      return __ptr.fetch_add(1, std::memory_order_relaxed);
    }

    void __cleanup_entry(wrapper_type &proc, proc::subprocess_result res) {
      if constexpr (process_wrapper_has_on_exit<wrapper_type>) {
        proc.on_exit(res);
      }
    }

  public:
    process_pool(chrono::milliseconds loop_timer = std::chrono::seconds(5)) :
      __ptr{0}, __processes{}, __run_loop{false}, __loop_timer{loop_timer},
      __t{&process_pool::__loop, this} {}

    size_t insert_process(wrapper_type wrapper) {
      return __processes.read_mut([&](std::vector<pool_entry> &processes) {
        size_t cur_id = __get_insert_id();
        processes.emplace_back(pool_entry{cur_id, std::move(wrapper)});
        /*
          Start the thread
        */
        if (cur_id == 0) {
          __run_loop.test_and_set();
          __run_loop.notify_one();
        }
        return cur_id;
      });
    }

    std::optional<pool_entry> unmanage(size_t id) {
      return __processes.read_mut(
        [&](std::vector<pool_entry> &processes) -> std::optional<pool_entry> {
          auto entry = std::ranges::find(processes, id, &pool_entry::id);
          if (entry == processes.end()) {
            return std::nullopt;
          } else {
            pool_entry unmanaged_entry = std::move(*entry);
            processes.erase(entry);
            return unmanaged_entry;
          }
        }
      );
    }

    auto stop(size_t id)
      requires(process_wrapper_has_stop<wrapper_type>)
    {
      return unmanage(id).transform([&](pool_entry &&entry) {
        return entry.process.stop().transform([&](proc::subprocess_result &&result) {
          __cleanup_entry(entry.process, result);
          return pool_result{
            .process{std::move(entry.process)}, .result{std::move(result)}, .id = entry.id
          };
        });
      });
    }

    std::optional<std::expected<pool_result, proc::subprocess_error>> kill(size_t id) {
      return unmanage(id).transform([&](pool_entry &&entry) {
        return process_wrapper::get_process(entry.process)
          .kill_and_wait(false)
          .transform([&](proc::subprocess_result res) {
            __cleanup_entry(entry.process, res);
            return pool_result{
              .id = entry.id, .process{std::move(entry.process)}, .result{std::move(res)}
            };
          });
      });
    }

    std::optional<std::expected<pool_result, proc::subprocess_error>> wait(
      size_t id, bool check = true
    ) {
      return unmanage(id).transform([&](pool_entry &&entry) {
        return process_wrapper::get_process(entry.process)
          .wait(check)
          .transform([&](proc::subprocess_result res) {
            __cleanup_entry(entry.process, res);
            return pool_result{
              .id = entry.id, .process{std::move(entry.process)}, .result{std::move(res)}
            };
          });
      });
    }

    size_t size() const noexcept {
      return __processes.read([](const auto &processes) { return processes.size(); });
    }

    void join(std::optional<std::chrono::milliseconds> timeout = std::nullopt) {
      while (size() != 0) {
        if (timeout) {
          std::this_thread::sleep_for(*timeout);
          __run_loop.clear();
        } else {
          std::this_thread::yield();
        }
      }
    }

    ~process_pool() {
      __run_loop.clear();
      __t.join();
    }
  };
}