#include <string_view>
#include <functional>

void zeromq_async_reply(std::string_view client_id, std::string_view reply_msg);
void zeromq_async_main(std::function<void(std::string_view, std::string_view)> process_request);
