#include "dorogin_v_contrasts_raising/seq/include/ops_seq.hpp"

#include <algorithm>

namespace dorogin_v_contrasts_raising {

namespace {
constexpr float kContrastBoost = 1.3F;
}

DoroginVContrastsRaisingSEQ::DoroginVContrastsRaisingSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput().resize(in.size());
}

bool DoroginVContrastsRaisingSEQ::ValidationImpl() {
  return true;
}

bool DoroginVContrastsRaisingSEQ::PreProcessingImpl() {
  return true;
}

bool DoroginVContrastsRaisingSEQ::RunImpl() {
  const auto &src = GetInput();
  auto &dst = GetOutput();

  for (size_t i = 0; i < src.size(); ++i) {
    const int value = static_cast<int>(src[i] * kContrastBoost);
    dst[i] = static_cast<uint8_t>(std::clamp(value, 0, 255));
  }
  return true;
}

bool DoroginVContrastsRaisingSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace dorogin_v_contrasts_raising
