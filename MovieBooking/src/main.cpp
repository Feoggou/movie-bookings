// MovieBooking.cpp : Defines the entry point for the application.
//

#include <nlohmann/json.hpp>

#include <iostream>

using namespace std;

void json_foo()
{
    std::string json_str = R"(["apple", "banana", "cherry"])";

    // Parse JSON string
    nlohmann::json j = nlohmann::json::parse(json_str);

    // Convert to vector
    std::vector<std::string> vec = j.get<std::vector<std::string>>();

    for (const auto& s : vec) {
        std::cout << s << std::endl;
    }

    // **************

    j = vec;

    // Convert JSON to string
    json_str = j.dump(4); // Optional: j.dump(4) for pretty-print

    std::cout << "JSON string: " << json_str << std::endl;
}

int main()
{
	std::cout << "Hello World!" << std::endl;

    json_foo();

	return 0;
}
