module;

#include "mb_service.hpp"

#include <functional>
#include <vector>

module mbooking:workers;

void start_workers();

void request_command(std::function<void()> &&func);
