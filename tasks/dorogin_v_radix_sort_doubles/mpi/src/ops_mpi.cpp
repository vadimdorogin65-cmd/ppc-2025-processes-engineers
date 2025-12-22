#include "dorogin_v_radix_sort_doubles/mpi/include/ops_mpi.hpp"

#include <mpi.h>

namespace dorogin_v_radix_sort_doubles {

bool DoroginVRadixSortDoublesMPI::ValidationImpl() {
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  return true;
}

bool DoroginVRadixSortDoublesMPI::PreProcessingImpl() {
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int global_size = 0;
  if (world_rank == 0) {
    global_size = static_cast<int>(GetInput().size());
  }

  MPI_Bcast(&global_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  counts.assign(world_size, 0);
  displs.assign(world_size, 0);

  const int base = global_size / world_size;
  const int rem = global_size % world_size;

  for (int i = 0; i < world_size; ++i) {
    counts[i] = base + (i < rem ? 1 : 0);
  }

  for (int i = 1; i < world_size; ++i) {
    displs[i] = displs[i - 1] + counts[i - 1];
  }

  local.assign(static_cast<std::size_t>(counts[world_rank]), 0.0);

  double* send_buf = nullptr;
  if (world_rank == 0 && global_size > 0) {
    send_buf = GetInput().data();
  }

  MPI_Scatterv(
      send_buf,
      counts.data(),
      displs.data(),
      MPI_DOUBLE,
      local.data(),
      counts[world_rank],
      MPI_DOUBLE,
      0,
      MPI_COMM_WORLD);

  GetOutput().clear();
  return true;
}

// ⚠️ RunImpl НЕ ТРОГАЕМ

bool DoroginVRadixSortDoublesMPI::PostProcessingImpl() {
  int out_size = 0;

  if (world_rank == 0) {
    out_size = static_cast<int>(local.size());
    GetOutput() = local;
  }

  MPI_Bcast(&out_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank != 0) {
    GetOutput().assign(static_cast<std::size_t>(out_size), 0.0);
  }

  if (out_size > 0) {
    MPI_Bcast(GetOutput().data(), out_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }

  return true;
}

}  // namespace dorogin_v_radix_sort_doubles
