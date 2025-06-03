module;

#include <functional>
#include <string_view>

// TODO: Only for VC++
#pragma warning(disable: 4844)
module mbooking;

namespace mbooking {

void create_service(std::function<void(std::string_view, std::string_view)> process_reply)
{
}

void zeromq_async_reply(std::string_view client_id, std::string_view reply_msg)
{
}

void zeromq_async_main(std::function<void(std::string_view, std::string_view)> process_request)
{
}

} // namespace mbooking
