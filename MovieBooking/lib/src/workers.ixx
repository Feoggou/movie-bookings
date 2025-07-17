module;

#include "mb_service.hpp"

#include <functional>
#include <vector>

export module mbooking:workers;

export void start_workers();

export void request_command(std::function<void()> &&func);



