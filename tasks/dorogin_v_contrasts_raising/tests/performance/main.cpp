#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <vector>

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace dorogin_v_contrasts_raising {

class DoroginVRunPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    constexpr size_t kSize = 1'000'000;
    input_.assign(kSize, 120);

    expected_.resize(kSize);
    constexpr float kFactor = 1.3F;

    std::transform(input_.begin(), input_.end(), expected_.begin(),
                   [](uint8_t v) { return static_cast<uint8_t>(std::clamp(static_cast<int>(v * kFactor), 0, 255)); });
  }

  bool CheckTestOutputData(OutType &out) override {
    return out.size() == expected_.size() && std::equal(out.begin(), out.end(), expected_.begin());
  }

  InType GetTestInputData() override {
    return input_;
  }

 private:
  InType input_;
  OutType expected_;
};

namespace {

TEST_P(DoroginVRunPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, DoroginVContrastsRaisingSEQ>(PPC_SETTINGS_dorogin_v_contrasts_raising);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = DoroginVRunPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, DoroginVRunPerfTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace dorogin_v_contrasts_raising
