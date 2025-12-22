#include "dorogin_v_contrast_raising/seq/include/ops_seq.hpp"

#include <algorithm>

namespace dorogin_v_contrast_raising {

DoroginVContrastRaisingSEQ::DoroginVContrastRaisingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool DoroginVContrastRaisingSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool DoroginVContrastRaisingSEQ::PreProcessingImpl() {
  std::fill(GetOutput().begin(), GetOutput().end(), 0);
  return true;
}

bool DoroginVContrastRaisingSEQ::RunImpl() {
  constexpr float factor = 1.3F;

  const auto &src = GetInput();
  auto &dst = GetOutput();

  for (std::size_t i = 0; i < src.size(); ++i) {
    int value = static_cast<int>(src[i] * factor);
    value = std::max(0, std::min(255, value));
    dst[i] = static_cast<uint8_t>(value);
  }

  return true;
}

bool DoroginVContrastRaisingSEQ::PostProcessingImpl() {
  return !GetOutput().empty();
}

}  // namespace dorogin_v_contrast_raising
