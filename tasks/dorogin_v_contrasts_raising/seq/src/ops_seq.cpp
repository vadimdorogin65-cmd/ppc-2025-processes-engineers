#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kContrastFactor = 1.3F;
}

DoroginVContrastsRaisingSEQ::DoroginVContrastsRaisingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool DoroginVContrastsRaisingSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool DoroginVContrastsRaisingSEQ::PreProcessingImpl() {
  GetOutput().assign(GetInput().size(), 0);
  return true;
}

bool DoroginVContrastsRaisingSEQ::RunImpl() {
  const auto &src = GetInput();
  auto &dst = GetOutput();

  for (size_t i = 0; i < src.size(); ++i) {
    const int adjusted = static_cast<int>(static_cast<float>(src[i]) * kContrastFactor);
    dst[i] = static_cast<uint8_t>(std::clamp(adjusted, 0, 255));
  }
  return true;
}

bool DoroginVContrastsRaisingSEQ::PostProcessingImpl() {
  return GetOutput().size() == GetInput().size();
}

}  // namespace dorogin_v_contrasts_raising
