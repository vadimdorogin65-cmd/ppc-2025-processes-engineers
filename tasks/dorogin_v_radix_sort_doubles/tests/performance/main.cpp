#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "dorogin_v_radix_sort_doubles/common/include/common.hpp"
#include "dorogin_v_radix_sort_doubles/mpi/include/ops_mpi.hpp"
#include "dorogin_v_radix_sort_doubles/seq/include/ops_seq.hpp"

#include "util/include/perf_test_util.hpp"

namespace dorogin_v_radix_sort_doubles {

class DoroginVRadixSortDoublesPerfTests
    : public ppc::util::BaseRunPerfTests<InType, OutType> {
 public:
  void SetUp() override {
    constexpr std::size_t kSize = 200000;
    input_data_.resize(kSize);

    int seed = 123;
    for (std::size_t i = 0; i < kSize; ++i) {
      seed = seed * 1103515245 + 12345;
      input_data_[i] = static_cast<double>(seed) / 1000.0;
    }

    expected_ = input_data_;
    std::sort(expected_.begin(), expected_.end());
  }

  bool CheckTestOutputData(OutType& output_data) final {
    return output_data == expected_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
    InType input_data_;
    OutType expected_;
};

TEST_P(DoroginVRadixSortDoublesPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<
        InType,
        DoroginVRadixSortDoublesMPI,
        DoroginVRadixSortDoublesSEQ>(
        PPC_SETTINGS_dorogin_v_radix_sort_doubles);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);
const auto kPerfTestName = DoroginVRadixSortDoublesPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests,
                         DoroginVRadixSortDoublesPerfTests,
                         kGtestValues,
                         kPerfTestName);

}  // namespace dorogin_v_radix_sort_doubles
