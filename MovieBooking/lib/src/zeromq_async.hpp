#pragma once

#include <functional>
#include <string>

void zeromq_async_main(std::function<std::string(std::string_view)> process_request);
