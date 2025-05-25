#pragma once

#include <functional>
#include <string>

void zeromq_async_main(std::function<void(std::string_view, std::string_view)> process_request);
void zeromq_async_reply(std::string_view request_id, std::string_view reply_msg);
