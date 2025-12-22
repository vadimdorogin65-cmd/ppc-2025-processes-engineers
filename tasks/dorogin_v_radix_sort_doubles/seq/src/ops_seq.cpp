#include "dorogin_v_radix_sort_doubles/seq/include/ops_seq.hpp"

#include <cstdint>
#include <cstring>
#include <vector>

namespace dorogin_v_radix_sort_doubles {

namespace {

uint64_t ToSortable(double x) {
  uint64_t u;
  std::memcpy(&u, &x, sizeof(double));
  return (u & (1ULL << 63)) ? ~u : (u ^ (1ULL << 63));
}

double FromSortable(uint64_t u) {
  u = (u & (1ULL << 63)) ? (u ^ (1ULL << 63)) : ~u;
  double x;
  std::memcpy(&x, &u, sizeof(double));
  return x;
}

void RadixSort(std::vector<double>* vec) {
  const std::size_t n = vec->size();
  std::vector<uint64_t> a(n), tmp(n);

  for (std::size_t i = 0; i < n; ++i)
    a[i] = ToSortable((*vec)[i]);

  for (int byte = 0; byte < 8; ++byte) {
    std::size_t count[256]{};

    for (auto v : a)
      ++count[(v >> (byte * 8)) & 0xFF];

    std::size_t pos[256]{};
    for (int i = 1; i < 256; ++i)
      pos[i] = pos[i - 1] + count[i - 1];

    for (auto v : a)
      tmp[pos[(v >> (byte * 8)) & 0xFF]++] = v;

    a.swap(tmp);
  }

  for (std::size_t i = 0; i < n; ++i)
    (*vec)[i] = FromSortable(a[i]);
}

}  // namespace

DoroginVRadixSortDoublesSEQ::DoroginVRadixSortDoublesSEQ(const InType& in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool DoroginVRadixSortDoublesSEQ::ValidationImpl() {
  return true;
}

bool DoroginVRadixSortDoublesSEQ::PreProcessingImpl() {
  data = GetInput();
  return true;
}

bool DoroginVRadixSortDoublesSEQ::RunImpl() {
  RadixSort(&data);
  return true;
}

bool DoroginVRadixSortDoublesSEQ::PostProcessingImpl() {
  GetOutput() = data;
  return true;
}

}  // namespace dorogin_v_radix_sort_doubles
