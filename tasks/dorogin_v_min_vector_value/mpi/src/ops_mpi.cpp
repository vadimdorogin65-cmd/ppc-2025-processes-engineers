#include "dorogin_v_min_vector_value/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <ranges>

#include "dorogin_v_min_vector_value/common/include/common.hpp"

namespace dorogin_v_min_vector_value {

DoroginVMinVectorValueMPI::DoroginVMinVectorValueMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool DoroginVMinVectorValueMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool DoroginVMinVectorValueMPI::PreProcessingImpl() {
  return true;
}

bool DoroginVMinVectorValueMPI::RunImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int global_min = 0;

  if (rank == 0) {
    const auto &data = GetInput();

    const auto it_min = std::ranges::min_element(data);
    global_min = (it_min != data.end()) ? *it_min : 0;
  }

  MPI_Bcast(&global_min, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = global_min;
  return true;
}

bool DoroginVMinVectorValueMPI::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_min_vector_value
