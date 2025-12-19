#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"

namespace dorogin_v_contrasts_raising {

class DoroginVRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &p) {
    return std::to_string(std::get<0>(p)) + "_" + std::get<1>(p);
  }

 protected:
  void SetUp() override {
    constexpr size_t kSize = 256;
    input_.resize(kSize);

    for (size_t i = 0; i < kSize; ++i) {
      input_[i] = static_cast<uint8_t>(i);
    }

    expected_.resize(kSize);
    constexpr float kFactor = 1.3F;

    std::ranges::transform(input_, expected_.begin(), [](uint8_t v) {
      const float scaled = static_cast<float>(v) * kFactor;
      const int adjusted = static_cast<int>(scaled);
      return static_cast<uint8_t>(std::clamp(adjusted, 0, 255));
    });
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

const std::array<TestType, 3> kParams = {
    std::make_tuple(0, "small"),
    std::make_tuple(1, "medium"),
    std::make_tuple(2, "large"),
};

const auto kTasks =
    ppc::util::AddFuncTask<DoroginVContrastsRaisingSEQ, InType>(kParams, PPC_SETTINGS_dorogin_v_contrasts_raising);

const auto kGtestValues = ppc::util::ExpandToValues(kTasks);

const auto kTestName = DoroginVRunFuncTests::PrintFuncTestName<DoroginVRunFuncTests>;

TEST_P(DoroginVRunFuncTests, ContrastFunctional) {
  ExecuteTest(GetParam());
}

INSTANTIATE_TEST_SUITE_P(ContrastFunctionalTests, DoroginVRunFuncTests, kGtestValues, kTestName);

}  // namespace

}  // namespace dorogin_v_contrasts_raising
