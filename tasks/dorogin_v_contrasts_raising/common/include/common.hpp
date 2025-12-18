#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace dorogin_v_contrasts_raising {

using InType = std::vector<uint8_t>;
using OutType = std::vector<uint8_t>;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace dorogin_v_contrasts_raising
