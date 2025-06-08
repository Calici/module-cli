module;
#include <rfl.hpp>
export module pharmaco.core.api:api_module;
import pharmaco.core.curl;
import :generic_context;

namespace pharmaco::core::api {
  export enum struct module_status { INIT, PARAM_ACCEPTED, STOP, ERROR, LOADING, COMPLETE };
  export enum struct module_update_type { GOOD, WARNING, ERROR, COMPLETE };

  template <class data_type, bool is_update>
  using updatable_data = std::conditional_t<is_update, std::optional<data_type>, data_type>;

  export template <bool is_update> struct api_module_data {
    updatable_data<double, is_update> progress;
    updatable_data<rfl::Generic::Object, is_update> parameters;
    updatable_data<std::optional<rfl::Generic::Object>, is_update> results;
    updatable_data<module_status, is_update> status;
    updatable_data<std::optional<module_update_type>, is_update> major_update_type;
    updatable_data<size_t, is_update> id; // always null. Never read.

    /*
      These are for extra data.
    */
    rfl::ExtraFields<rfl::Generic> extra_fields;
  };

  export class api_module {
    api_module_data<false> __data;
    api_module_data<true> __update_data;
    module_context __ctx;
    api_module(api_module_data<false> data, module_context ctx) :
      __data{std::move(data)}, __update_data{}, __ctx{std::move(ctx)} {}

  public:
    const api_module_data<false> &data() const {
      return __data;
    }
    api_module &set_progress(double progress) {
      __data.progress = progress;
      __update_data.progress = progress;
      return *this;
    }
    api_module &set_results(const rfl::Generic::Object &results) {
      __data.results = results;
      __update_data.results = results;
      return *this;
    }
    api_module &set_major_update_type(const module_update_type &update) {
      __data.major_update_type = update;
      __update_data.major_update_type = update;
      return *this;
    }
    api_module &set_status(module_status status) {
      __data.status = status;
      __update_data.status = status;
      return *this;
    }
    std::expected<void, curl_http_error> push() {
      return __ctx
        .make_request<api_module_data<false>>(
          __ctx.get_url("{}/module/{}/", __ctx.extras.id),
          curl_http_request_method::POST,
          __update_data
        )
        .transform([&](auto &&resp) {
          __data = std::move(resp.data);
          __update_data = api_module_data<true>{};
        });
    }

    static std::expected<api_module, curl_http_error> make(
      const module_context &ctx = module_context::get_global_context()
    ) {
      return ctx
        .make_request<api_module_data<false>>(
          ctx.get_url("{}/module/{}", ctx.extras.id), curl_http_request_method::GET
        )
        .transform([&](auto &&resp) { return api_module{std::move(resp.data), ctx}; });
    }
  };
};