#pragma once

#include "dorogin_v_radix_sort_doubles/common/include/common.hpp"

namespace dorogin_v_radix_sort_doubles {

class DoroginVRadixSortDoublesMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }

  explicit DoroginVRadixSortDoublesMPI(const InType& in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace dorogin_v_radix_sort_doubles
