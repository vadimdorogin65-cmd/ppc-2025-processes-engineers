#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "dorogin_v_contrast_raising/common/include/common.hpp"
#include "dorogin_v_contrast_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrast_raising/seq/include/ops_seq.hpp"

namespace dorogin_v_contrast_raising {

class DoroginVContrastRaisingPerfTests : public ::testing::Test {
 protected:
  InType input;
  OutType reference;

  void SetUp() override {
    constexpr size_t kSize = 50'000'000;
    input.resize(kSize);
    for (size_t i = 0; i < kSize; ++i) {
      input[i] = static_cast<uint8_t>(i);
    }

    reference.resize(kSize);
    for (size_t i = 0; i < kSize; ++i) {
      int v = static_cast<int>(static_cast<float>(input[i]) * 1.3F);
      reference[i] = static_cast<uint8_t>(std::clamp(v, 0, 255));
    }
  }
};

TEST_F(DoroginVContrastRaisingPerfTests, SeqPipeline) {
  DoroginVContrastRaisingSEQ task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), reference);
}

TEST_F(DoroginVContrastRaisingPerfTests, MpiPipeline) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  DoroginVContrastRaisingMPI task(input);
  ASSERT_TRUE(task.Validation());
  ASSERT_TRUE(task.PreProcessing());
  ASSERT_TRUE(task.Run());
  ASSERT_TRUE(task.PostProcessing());

  if (rank == 0) {
    EXPECT_EQ(task.GetOutput(), reference);
  }
}

TEST(IncreaseContrastEdge, EmptyInputSeq) {
  InType empty;
  DoroginVContrastRaisingSEQ task(empty);
  EXPECT_FALSE(task.Validation());
}

TEST(IncreaseContrastEdge, EmptyInputMpi) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType empty;
  DoroginVContrastRaisingMPI task(empty);
  EXPECT_FALSE(task.Validation());
}

}  // namespace dorogin_v_contrast_raising
