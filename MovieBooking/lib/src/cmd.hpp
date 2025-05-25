#pragma once

#include <mbooking/movie_booking.h>

#include <nlohmann/json.hpp>

#include <string>
#include <functional>


namespace movie_booking {

void execute_command(const ID &id, const std::string& command_name, const nlohmann::json& args);

void start_reply_thread(std::function<void(std::string_view, std::string_view)> process_reply);

void create_service(std::function<void(std::string_view, std::string_view)> process_reply);

} // namespace movie_booking