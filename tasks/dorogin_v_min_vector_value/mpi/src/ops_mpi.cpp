#include "dorogin_v_min_vector_value/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <limits>
#include <vector>

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
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int global_size = 0;
  if (rank == 0) {
    global_size = static_cast<int>(GetInput().size());
  }

  MPI_Bcast(&global_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (global_size == 0) {
    if (rank == 0) {
      GetOutput() = 0;
    }
    return true;
  }

  const int base_block = global_size / size;
  const int remainder = global_size % size;

  const int local_size = base_block + (rank < remainder ? 1 : 0);

  std::vector<int> sendcounts(size);
  std::vector<int> displs(size);

  if (rank == 0) {
    int offset = 0;
    for (int proc_idx = 0; proc_idx < size; ++proc_idx) {
      const int cnt = base_block + (proc_idx < remainder ? 1 : 0);
      sendcounts[proc_idx] = cnt;
      displs[proc_idx] = offset;
      offset += cnt;
    }
  }

  MPI_Bcast(sendcounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_data(local_size);

  MPI_Scatterv(rank == 0 ? GetInput().data() : nullptr, sendcounts.data(), displs.data(), MPI_INT, local_data.data(),
               local_size, MPI_INT, 0, MPI_COMM_WORLD);

  int local_min = std::numeric_limits<int>::max();
  for (int v : local_data) {
    local_min = std::min(local_min, v);
  }

  int global_min = 0;
  MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

  GetOutput() = global_min;
  return true;
}

bool DoroginVMinVectorValueMPI::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_min_vector_value
