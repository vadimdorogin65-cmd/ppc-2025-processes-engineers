#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace dorogin_v_min_vector_value {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace dorogin_v_min_vector_value
