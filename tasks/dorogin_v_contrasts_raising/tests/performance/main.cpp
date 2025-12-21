#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "dorogin_v_contrasts_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kFactor = 1.3F;
} // namespace

class DoroginVPerformanceTests : public ::testing::Test {
 protected:
  InType input;
  OutType expected;

  void SetUp() override {
    constexpr size_t kSize = 1'000'000;
    input.assign(kSize, 128);

    expected.resize(kSize);
    for (size_t i = 0; i < kSize; ++i) {
      const int v = static_cast<int>(128 * kFactor);
      expected[i] = static_cast<uint8_t>(std::clamp(v, 0, 255));
    }
  }
};

TEST_F(DoroginVPerformanceTests, SeqFullCycle) {
  DoroginVContrastsRaisingSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), expected);
}

TEST_F(DoroginVPerformanceTests, MpiFullCycle) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  DoroginVContrastsRaisingMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());

  if (rank == 0) {
    EXPECT_EQ(task.GetOutput(), expected);
  }
}

TEST_F(DoroginVPerformanceTests, MpiLargeData) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  constexpr size_t kLarge = 5'000'000;
  InType data(kLarge, 200);

  DoroginVContrastsRaisingMPI task(data);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());

  if (rank == 0) {
    EXPECT_EQ(task.GetOutput().size(), kLarge);
  }
}

}  // namespace dorogin_v_contrasts_raising
