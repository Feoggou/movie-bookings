#pragma once

#include "mb_service.hpp"

#include <functional>
#include <vector>

void start_workers();

void request_command(std::function<void()> &&func);
