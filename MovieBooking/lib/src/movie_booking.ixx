module;

#include "mb_service.hpp"
#include "cmd.hpp"
#include "workers.hpp"

#include <zmq_addon.hpp>

#include <functional>
#include <string_view>
#include <memory>
#include <iostream>

// TODO: Only for VC++
#pragma warning(disable: 4844)
module mbooking;

import :api;

namespace mbooking {

std::unique_ptr<movie_booking::API> g_API;

namespace {
    movie_booking::SyncedService service;
}

void create_service(std::function<void(std::string_view, std::string_view)> process_reply)
{
    g_API = std::make_unique<movie_booking::API>(service);

    movie_booking::start_reply_thread(process_reply);
    ::start_workers();
}

} // namespace mbooking
