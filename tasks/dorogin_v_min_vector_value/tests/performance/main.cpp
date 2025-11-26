#include <gtest/gtest.h>

#include "dorogin_v_min_vector_value/common/include/common.hpp"
#include "dorogin_v_min_vector_value/mpi/include/ops_mpi.hpp"
#include "dorogin_v_min_vector_value/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace dorogin_v_min_vector_value {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, DoroginVMinVectorValueMPI, DoroginVMinVectorValueSEQ>(PPC_SETTINGS_dorogin_v_min_vector_value);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace dorogin_v_min_vector_value
