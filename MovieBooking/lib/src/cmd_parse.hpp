#pragma once

#include <nlohmann/json.hpp>

#include <string>


namespace movie_booking {

void execute_command(const std::string& command_name, const nlohmann::json& args);

void start_reply_thread();

void create_service();

} // namespace movie_booking