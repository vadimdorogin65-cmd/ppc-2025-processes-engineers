#pragma once

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "task/include/task.hpp"

namespace dorogin_v_contrasts_raising {

class DoroginVContrastsRaisingSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }

  explicit DoroginVContrastsRaisingSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace dorogin_v_contrasts_raising
