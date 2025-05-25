#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <functional>


namespace movie_booking {

void execute_command(std::string_view request_id, std::function<void(std::string_view request_id, std::string_view reply_msg)> process_reply, const std::string& command_name, const nlohmann::json& args);

void start_reply_thread(std::function<void(std::string_view request_id, std::string_view reply_msg)> process_reply);

void create_service(std::function<void(std::string_view request_id, std::string_view reply_msg)> process_reply);

} // namespace movie_booking