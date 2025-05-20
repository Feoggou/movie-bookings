#pragma once

#include <ranges>
#include <optional>

namespace utils {

    template <std::ranges::input_range Range, typename Pred>
    std::optional<std::ranges::range_value_t<Range>> find_if_optional(const Range& r, Pred pred)
    {
        auto it = std::ranges::find_if(r, pred);
        if (it != std::ranges::end(r))
            return *it;
        return std::nullopt;
    }

} // utils
