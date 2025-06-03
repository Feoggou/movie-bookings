#pragma once

#include <functional>
#include <string>
#include <string_view>

void run_zero_mq_server(std::function<std::string (std::string_view)> process_request);
