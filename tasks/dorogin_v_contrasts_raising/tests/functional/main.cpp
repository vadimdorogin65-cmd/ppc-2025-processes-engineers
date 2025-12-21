#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "dorogin_v_contrasts_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kFactor = 1.3F;
}  // namespace

class DoroginVFunctionalTests : public ::testing::Test {
 protected:
  InType input;
  OutType expected;

  void SetUp() override {
    constexpr size_t kSize = 512;
    input.resize(kSize);
    for (size_t i = 0; i < kSize; ++i) {
      input[i] = static_cast<uint8_t>(i % 256);
    }

    expected.resize(kSize);
    for (size_t i = 0; i < kSize; ++i) {
      const int v = static_cast<int>(static_cast<float>(input[i]) * kFactor);
      expected[i] = static_cast<uint8_t>(std::clamp(v, 0, 255));
    }
  }
};

TEST_F(DoroginVFunctionalTests, SeqFullCycle) {
  DoroginVContrastsRaisingSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), expected);
}

TEST_F(DoroginVFunctionalTests, SeqRunOnly) {
  DoroginVContrastsRaisingSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
}

TEST_F(DoroginVFunctionalTests, MpiFullCycle) {
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

TEST_F(DoroginVFunctionalTests, MpiRunOnly) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  DoroginVContrastsRaisingMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());

  if (rank == 0) {
    EXPECT_EQ(task.GetOutput(), expected);
  }
}

TEST(DoroginVEdgeCases, SeqEmptyInput) {
  InType empty;
  DoroginVContrastsRaisingSEQ task(empty);
  EXPECT_FALSE(task.Validation());
}

TEST(DoroginVEdgeCases, MpiEmptyInput) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType empty;
  DoroginVContrastsRaisingMPI task(empty);
  EXPECT_FALSE(task.Validation());
}

TEST(DoroginVEdgeCases, MpiUnevenSize) {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  InType data(static_cast<size_t>((size * 3) + 1), 100);
  DoroginVContrastsRaisingMPI task(data);

  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());

  if (rank == 0) {
    for (uint8_t v : task.GetOutput()) {
      EXPECT_EQ(v, static_cast<uint8_t>(std::clamp(int(100 * kFactor), 0, 255)));
    }
  }
}

}  // namespace dorogin_v_contrasts_raising
