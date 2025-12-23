#include "dorogin_v_contrast_raising/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "dorogin_v_contrast_raising/common/include/common.hpp"

namespace dorogin_v_contrast_raising {

DoroginVContrastRaisingMPI::DoroginVContrastRaisingMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool DoroginVContrastRaisingMPI::ValidationImpl() {
  return !GetInput().empty();
}

bool DoroginVContrastRaisingMPI::PreProcessingImpl() {
  std::fill(GetOutput().begin(), GetOutput().end(), 0);
  return true;
}

bool DoroginVContrastRaisingMPI::RunImpl() {
  int rank = 0;
  int world_size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  const std::size_t total_size = GetInput().size();

  std::vector<int> block_sizes(world_size, 0);
  std::vector<int> offsets(world_size, 0);

  const std::size_t base = total_size / world_size;
  const std::size_t tail = total_size % world_size;

  for (int i = 0; i < world_size; ++i) {
    block_sizes[i] = static_cast<int>(base + (std::cmp_less(i, tail) ? 1 : 0));
  }

  for (int i = 1; i < world_size; ++i) {
    offsets[i] = offsets[i - 1] + block_sizes[i - 1];
  }

  std::vector<uint8_t> local_input(block_sizes[rank]);
  std::vector<uint8_t> local_output(block_sizes[rank]);

  MPI_Scatterv(rank == 0 ? GetInput().data() : nullptr, block_sizes.data(), offsets.data(), MPI_UNSIGNED_CHAR,
               local_input.data(), block_sizes[rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  constexpr float kFactor = 1.3F;
  for (std::size_t i = 0; i < local_input.size(); ++i) {
    int scaled = static_cast<int>(static_cast<float>(local_input[i]) * kFactor);
    scaled = std::clamp(scaled, 0, 255);
    local_output[i] = static_cast<uint8_t>(scaled);
  }

  if (world_size == 1) {
    std::copy(local_output.begin(), local_output.end(), GetOutput().begin());
  } else {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(local_output.data(), block_sizes[rank], MPI_UNSIGNED_CHAR, rank == 0 ? GetOutput().data() : nullptr,
                block_sizes.data(), offsets.data(), MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
  }

  return true;
}

bool DoroginVContrastRaisingMPI::PostProcessingImpl() {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    return !GetOutput().empty();
  }
  return true;
}

}  // namespace dorogin_v_contrast_raising
