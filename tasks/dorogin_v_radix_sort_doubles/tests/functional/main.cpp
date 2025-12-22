#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <string>
#include <tuple>
#include <vector>

#include "dorogin_v_radix_sort_doubles/common/include/common.hpp"
#include "dorogin_v_radix_sort_doubles/mpi/include/ops_mpi.hpp"
#include "dorogin_v_radix_sort_doubles/seq/include/ops_seq.hpp"

#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace dorogin_v_radix_sort_doubles {

class DoroginVRadixSortDoublesFuncTests
    : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType& test_param) {
    return std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    const auto& params =
        std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    input_data_ = std::get<0>(params);
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

namespace {

TEST_P(DoroginVRadixSortDoublesFuncTests, RadixSortDouble) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 7> kTestParam = {
    std::make_tuple(InType{}, "empty"),
    std::make_tuple(InType{1.0}, "single"),
    std::make_tuple(InType{1.0, 2.0, 3.0, 4.0}, "already_sorted"),
    std::make_tuple(InType{4.0, 3.0, 2.0, 1.0}, "reverse_sorted"),
    std::make_tuple(InType{2.2, 1.1, 2.2, 0.0, -1.5}, "duplicates"),
    std::make_tuple(InType{10.5, -3.2, 7.7, 0.0, -100.1}, "mixed"),
    std::make_tuple(InType{2.0, 1.0}, "two_elements"),
};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<DoroginVRadixSortDoublesMPI, InType>(
        kTestParam, PPC_SETTINGS_dorogin_v_radix_sort_doubles),
    ppc::util::AddFuncTask<DoroginVRadixSortDoublesSEQ, InType>(
        kTestParam, PPC_SETTINGS_dorogin_v_radix_sort_doubles));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kFuncTestName =
    DoroginVRadixSortDoublesFuncTests::PrintFuncTestName<DoroginVRadixSortDoublesFuncTests>;

INSTANTIATE_TEST_SUITE_P(RadixSortDoublesFuncTests,
                         DoroginVRadixSortDoublesFuncTests,
                         kGtestValues,
                         kFuncTestName);

}  // namespace

}  // namespace dorogin_v_radix_sort_doubles
