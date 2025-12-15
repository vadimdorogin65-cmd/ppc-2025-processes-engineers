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

  // Размер глобального вектора только на ранге 0
  int global_size = 0;
  if (rank == 0) {
    global_size = static_cast<int>(GetInput().size());
  }

  // Разослать размер всем
  MPI_Bcast(&global_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (global_size == 0) {
    if (rank == 0) {
      GetOutput() = 0;
    }
    return true;
  }

  // Базовый размер блока и остаток
  const int base_block = global_size / size;
  const int remainder = global_size % size;

  // Сколько элементов у текущего процесса
  const int local_size = base_block + (rank < remainder ? 1 : 0);

  // Массивы sendcounts и displacements только на ранге 0
  std::vector<int> sendcounts(size);
  std::vector<int> displs(size);

  if (rank == 0) {
    int offset = 0;
    for (int p = 0; p < size; ++p) {
      const int cnt = base_block + (p < remainder ? 1 : 0);
      sendcounts[p] = cnt;
      displs[p] = offset;
      offset += cnt;
    }
  }

  // Разослать sendcounts и displs всем
  MPI_Bcast(sendcounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

  // Локальный буфер
  std::vector<int> local_data(local_size);

  // Раздать данные по процессам
  MPI_Scatterv(rank == 0 ? GetInput().data() : nullptr,
               sendcounts.data(), displs.data(), MPI_INT,
               local_data.data(), local_size, MPI_INT,
               0, MPI_COMM_WORLD);

  // Локальный минимум
  int local_min = std::numeric_limits<int>::max();
  for (int v : local_data) {
    local_min = std::min(local_min, v);
  }

  // Глобальный минимум сразу всем (Allreduce)
  int global_min = 0;
  MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

  GetOutput() = global_min;
  return true;
}

bool DoroginVMinVectorValueMPI::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_min_vector_value
