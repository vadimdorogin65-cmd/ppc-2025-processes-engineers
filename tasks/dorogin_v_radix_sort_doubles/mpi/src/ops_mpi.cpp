#include "dorogin_v_radix_sort_doubles/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

// объявляем RadixSort из seq
namespace dorogin_v_radix_sort_doubles {
void RadixSort(std::vector<double>& vec);
}

namespace dorogin_v_radix_sort_doubles {

DoroginVRadixSortDoublesMPI::DoroginVRadixSortDoublesMPI(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().clear();
}

bool DoroginVRadixSortDoublesMPI::ValidationImpl() {
  return true;
}

bool DoroginVRadixSortDoublesMPI::PreProcessingImpl() {
  return true;
}

bool DoroginVRadixSortDoublesMPI::RunImpl() {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int n = static_cast<int>(GetInput().size());

  std::vector<int> counts(size, 0);
  std::vector<int> displs(size, 0);

  const int base = n / size;
  const int rem = n % size;

  for (int i = 0; i < size; ++i) {
    counts[i] = base + (i < rem ? 1 : 0);
  }

  for (int i = 1; i < size; ++i) {
    displs[i] = displs[i - 1] + counts[i - 1];
  }

  std::vector<double> local(static_cast<std::size_t>(counts[rank]));

  MPI_Scatterv(rank == 0 ? GetInput().data() : nullptr,
               counts.data(),
               displs.data(),
               MPI_DOUBLE,
               local.data(),
               counts[rank],
               MPI_DOUBLE,
               0,
               MPI_COMM_WORLD);

  // локальная сортировка
  RadixSort(local);

  std::vector<double> gathered;
  if (rank == 0) {
    gathered.resize(static_cast<std::size_t>(n));
  }

  MPI_Gatherv(local.data(),
              counts[rank],
              MPI_DOUBLE,
              rank == 0 ? gathered.data() : nullptr,
              counts.data(),
              displs.data(),
              MPI_DOUBLE,
              0,
              MPI_COMM_WORLD);

  if (rank == 0) {
    // простое последовательное слияние
    for (int i = 1; i < size; ++i) {
      std::inplace_merge(
          gathered.begin(),
          gathered.begin() + displs[i],
          gathered.begin() + displs[i] + counts[i]);
    }
    GetOutput() = gathered;
  }

  return true;
}

bool DoroginVRadixSortDoublesMPI::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_radix_sort_doubles
