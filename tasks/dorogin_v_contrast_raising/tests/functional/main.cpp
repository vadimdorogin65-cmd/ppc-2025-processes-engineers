#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>

#include "dorogin_v_contrast_raising/common/include/common.hpp"
#include "dorogin_v_contrast_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrast_raising/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"

namespace dorogin_v_contrast_raising {

class DoroginVRunFuncTestsContrastRaising : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<0>(test_param);
  }

 protected:
  void SetUp() override {
    constexpr std::size_t kSize = 1'000'000;
    input_data_.resize(kSize);
    reference_.resize(kSize);

    for (std::size_t i = 0; i < kSize; ++i) {
      input_data_[i] = static_cast<uint8_t>(i % 256);
    }

    constexpr float kFactor = 1.3F;
    for (std::size_t i = 0; i < kSize; ++i) {
      int v = static_cast<int>(static_cast<float>(input_data_[i]) * kFactor);
      v = std::clamp(v, 0, 255);
      reference_[i] = static_cast<uint8_t>(v);
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == reference_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType reference_;
};

namespace {

TEST_P(DoroginVRunFuncTestsContrastRaising, IncreaseContrast) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 1> kTestParams = {std::make_tuple(std::string("default"))};

const auto kTasks = std::tuple_cat(
    ppc::util::AddFuncTask<DoroginVContrastRaisingMPI, InType>(kTestParams, PPC_SETTINGS_dorogin_v_contrast_raising),
    ppc::util::AddFuncTask<DoroginVContrastRaisingSEQ, InType>(kTestParams, PPC_SETTINGS_dorogin_v_contrast_raising));

const auto kGtestValues = ppc::util::ExpandToValues(kTasks);

const auto kFuncTestName = DoroginVRunFuncTestsContrastRaising::PrintFuncTestName<DoroginVRunFuncTestsContrastRaising>;

INSTANTIATE_TEST_SUITE_P(ContrastRaisingFunc, DoroginVRunFuncTestsContrastRaising, kGtestValues, kFuncTestName);

}  // namespace

}  // namespace dorogin_v_contrast_raising
