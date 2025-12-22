#pragma once

#include <vector>
#include "task/include/task.hpp"

namespace dorogin_v_radix_sort_doubles {

using InType = std::vector<double>;
using OutType = std::vector<double>;
using BaseTask = ppc::task::Task<InType, OutType>;

class DoroginVRadixSortDoublesMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }

  explicit DoroginVRadixSortDoublesMPI(const InType& in) {
    SetTypeOfTask(GetStaticTypeOfTask());
    GetInput() = in;
    GetOutput() = {};
  }

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  // 🔴 ВАЖНО: эти поля должны быть ЗДЕСЬ
  std::vector<double> local;
  std::vector<int> counts;
  std::vector<int> displs;
  int world_rank{0};
  int world_size{1};
};

}  // namespace dorogin_v_radix_sort_doubles
