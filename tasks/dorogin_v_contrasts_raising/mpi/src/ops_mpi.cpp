#include "dorogin_v_contrasts_raising/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kContrastFactor = 1.3F;
}  // namespace

DoroginVContrastsRaisingMPI::DoroginVContrastsRaisingMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool DoroginVContrastsRaisingMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool DoroginVContrastsRaisingMPI::PreProcessingImpl() {
  GetOutput().assign(GetInput().size(), 0);
  return true;
}

bool DoroginVContrastsRaisingMPI::RunImpl() {
  int rank = 0;
  int comm_size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  const size_t total = GetInput().size();

  std::vector<int> counts(comm_size, static_cast<int>(total / comm_size));
  const int remainder = static_cast<int>(total % comm_size);
  for (int i = 0; i < remainder; ++i) {
    counts[i]++;
  }

  std::vector<int> offsets(comm_size, 0);
  std::partial_sum(counts.begin(), counts.end() - 1, offsets.begin() + 1);

  std::vector<uint8_t> local_input(counts[rank]);
  std::vector<uint8_t> local_output(counts[rank]);

  const uint8_t *sendbuf = rank == 0 ? GetInput().data() : nullptr;

  MPI_Scatterv(sendbuf, counts.data(), offsets.data(), MPI_UNSIGNED_CHAR, local_input.data(), counts[rank],
               MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  for (size_t i = 0; i < local_input.size(); ++i) {
    const float scaled = static_cast<float>(local_input[i]) * kContrastFactor;
    const int adjusted = static_cast<int>(scaled);
    local_output[i] = static_cast<uint8_t>(std::clamp(adjusted, 0, 255));
  }

  uint8_t *recvbuf = rank == 0 ? GetOutput().data() : nullptr;

  MPI_Gatherv(local_output.data(), counts[rank], MPI_UNSIGNED_CHAR, recvbuf, counts.data(), offsets.data(),
              MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  return true;
}

bool DoroginVContrastsRaisingMPI::PostProcessingImpl() {
  return GetOutput().size() == GetInput().size();
}

}  // namespace dorogin_v_contrasts_raising
