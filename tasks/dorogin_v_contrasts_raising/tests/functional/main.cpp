#include <gtest/gtest.h>
#include <mpi.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "dorogin_v_contrasts_raising/common/include/common.hpp"
#include "dorogin_v_contrasts_raising/mpi/include/ops_mpi.hpp"
#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace dorogin_v_contrasts_raising {

class DoroginVRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &p) {
    return std::to_string(std::get<0>(p)) + "_" + std::get<1>(p);
  }

 protected:
  void SetUp() override {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int w = 0;
    int h = 0;
    int c = 0;

    std::vector<uint8_t> tmp;

    if (rank == 0) {
      const std::string img_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_dorogin_v_contrasts_raising, "pic.jpg");

      uint8_t *data = stbi_load(img_path.c_str(), &w, &h, &c, STBI_rgb);
      ASSERT_NE(data, nullptr);

      const size_t size = static_cast<size_t>(w) * static_cast<size_t>(h) * STBI_rgb;

      tmp.assign(data, data + size);
      stbi_image_free(data);
    }

    // Рассылаем размеры
    MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&h, 1, MPI_INT, 0, MPI_COMM_WORLD);

    const size_t size = static_cast<size_t>(w) * static_cast<size_t>(h) * STBI_rgb;

    input_.resize(size);

    // Рассылаем изображение
    if (rank == 0) {
      input_ = tmp;
    }

    MPI_Bcast(input_.data(), static_cast<int>(size), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // Эталон
    expected_.resize(size);
    constexpr float kFactor = 1.3F;

    std::transform(input_.begin(), input_.end(), expected_.begin(),
                   [](uint8_t v) { return static_cast<uint8_t>(std::clamp(static_cast<int>(v * kFactor), 0, 255)); });
  }

  bool CheckTestOutputData(OutType &out) override {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank != 0) {
      return true;
    }

    if (out.size() != expected_.size()) {
      return false;
    }

    return std::equal(out.begin(), out.end(), expected_.begin());
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

const auto kTasks = std::tuple_cat(
    ppc::util::AddFuncTask<DoroginVContrastsRaisingMPI, InType>(kParams, PPC_SETTINGS_dorogin_v_contrasts_raising),
    ppc::util::AddFuncTask<DoroginVContrastsRaisingSEQ, InType>(kParams, PPC_SETTINGS_dorogin_v_contrasts_raising));

const auto kGtestValues = ppc::util::ExpandToValues(kTasks);

const auto kTestName = DoroginVRunFuncTests::PrintFuncTestName<DoroginVRunFuncTests>;

TEST_P(DoroginVRunFuncTests, ContrastFunctional) {
  ExecuteTest(GetParam());
}

INSTANTIATE_TEST_SUITE_P(ContrastFunctionalTests, DoroginVRunFuncTests, kGtestValues, kTestName);

}  // namespace

}  // namespace dorogin_v_contrasts_raising
