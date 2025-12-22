#pragma once

#include "dorogin_v_radix_sort_doubles/common/include/common.hpp"

namespace dorogin_v_radix_sort_doubles {

class DoroginVRadixSortDoublesSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }

  explicit DoroginVRadixSortDoublesSEQ(const InType& in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  InType data_;
};

}  // namespace dorogin_v_radix_sort_doubles
