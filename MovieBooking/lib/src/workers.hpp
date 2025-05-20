#include <functional>


void start_workers();

void request_command(std::function<void()> func);
