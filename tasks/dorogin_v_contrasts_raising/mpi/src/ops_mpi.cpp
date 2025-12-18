#include "dorogin_v_contrasts_raising/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kContrastBoost = 1.3F;
}

DoroginVContrastsRaisingMPI::DoroginVContrastsRaisingMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool DoroginVContrastsRaisingMPI::ValidationImpl() {
  return true;
}

bool DoroginVContrastsRaisingMPI::PreProcessingImpl() {
  return true;
}

bool DoroginVContrastsRaisingMPI::RunImpl() {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const size_t total_size = GetInput().size();

  std::vector<int> counts(size, static_cast<int>(total_size / size));
  for (int i = 0; i < static_cast<int>(total_size % size); ++i) {
    counts[i]++;
  }

  std::vector<int> displs(size, 0);
  for (int i = 1; i < size; ++i) {
    displs[i] = displs[i - 1] + counts[i - 1];
  }

  std::vector<uint8_t> local_in(counts[rank]);
  std::vector<uint8_t> local_out(counts[rank]);

  const uint8_t *sendbuf = (rank == 0 && !GetInput().empty()) ? GetInput().data() : nullptr;

  MPI_Scatterv(sendbuf, counts.data(), displs.data(), MPI_UNSIGNED_CHAR, local_in.data(), counts[rank],
               MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  for (size_t i = 0; i < local_in.size(); ++i) {
    const int value = static_cast<int>(local_in[i] * kContrastBoost);
    local_out[i] = static_cast<uint8_t>(std::clamp(value, 0, 255));
  }

  uint8_t *recvbuf = (rank == 0 && !GetOutput().empty()) ? GetOutput().data() : nullptr;

  MPI_Gatherv(local_out.data(), counts[rank], MPI_UNSIGNED_CHAR, recvbuf, counts.data(), displs.data(),
              MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  return true;
}

bool DoroginVContrastsRaisingMPI::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_contrasts_raising
