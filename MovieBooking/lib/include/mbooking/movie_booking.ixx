module;

#include <functional>
#include <string_view>

export module mbooking;
export import :workers;

export namespace mbooking {
    void create_service(std::function<void(std::string_view, std::string_view)> process_reply);
} // namespace mbooking
