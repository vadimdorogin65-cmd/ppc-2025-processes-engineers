#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include "dorogin_v_contrast_raising/common/include/common.hpp"
#include "dorogin_v_contrast_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrast_raising/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace dorogin_v_contrast_raising {

class DoroginVRunPerfTestsContrastRaising : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    input_data_.resize(kSize);
    for (std::size_t i = 0; i < kSize; ++i) {
      input_data_[i] = static_cast<uint8_t>(i % 256);
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return !output_data.empty() && output_data.size() == input_data_.size();
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  static constexpr std::size_t kSize = 1'000'000;
  InType input_data_;
};

TEST_P(DoroginVRunPerfTestsContrastRaising, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, DoroginVContrastRaisingMPI, DoroginVContrastRaisingSEQ>(
    PPC_SETTINGS_dorogin_v_contrast_raising);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = DoroginVRunPerfTestsContrastRaising::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(ContrastRaisingPerf, DoroginVRunPerfTestsContrastRaising, kGtestValues, kPerfTestName);

}  // namespace dorogin_v_contrast_raising
