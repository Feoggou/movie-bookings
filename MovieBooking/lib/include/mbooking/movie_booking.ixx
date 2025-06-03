module;

#include <functional>
#include <string_view>

export module mbooking;


export namespace mbooking {
    void create_service(std::function<void(std::string_view, std::string_view)> process_reply);

    void zeromq_async_main(std::function<void(std::string_view, std::string_view)> process_request);
    void zeromq_async_reply(std::string_view client_id, std::string_view reply_msg);
} // namespace mbooking
