#pragma once
#if defined(__BMI2__) || defined(__AVX2__)
#include <immintrin.h>
#include <stdint.h>

namespace libmorton
{

namespace bmi2_detail
{
inline uint32_t pdep(uint32_t source, uint32_t mask) noexcept
{
  return _pdep_u32(source, mask);
}
inline uint64_t pdep(uint64_t source, uint64_t mask) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
  return _pdep_u64(source, mask);
#else
  // Use the 32-bit intrinsic on 32-bit architectures
  uint32_t lower_source = static_cast<uint32_t>(source);
  uint32_t lower_mask = static_cast<uint32_t>(mask);

  uint32_t upper_source = static_cast<uint32_t>(source >> 32);
  uint32_t upper_mask = static_cast<uint32_t>(mask >> 32);

  // Extract results for the lower and upper parts
  uint32_t lower_result = _pdep_u32(lower_source, lower_mask);
  uint32_t upper_result = _pdep_u32(upper_source, upper_mask);

  // Combine results
  uint64_t combined_result = static_cast<uint64_t>(lower_result) |
    (static_cast<uint64_t>(upper_result) << (__builtin_popcount(lower_mask)));
  return combined_result;

#if 0
  // Use the 32-bit intrinsic on 32-bit architectures
  uint32_t lower_result = _pdep_u32(static_cast<uint32_t>(source), static_cast<uint32_t>(mask));
  uint32_t upper_result =
    _pdep_u32(static_cast<uint32_t>(source >> 32), static_cast<uint32_t>(mask >> 32));
  return static_cast<uint64_t>(lower_result) | (static_cast<uint64_t>(upper_result) << 32);

#endif
#endif
}
inline uint32_t pext(uint32_t source, uint32_t mask) noexcept
{
  return _pext_u32(source, mask);
}
inline uint64_t pext(uint64_t source, uint64_t mask) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
  // Use the 64-bit intrinsic on 64-bit architectures
  return _pext_u64(source, mask);
#else
  // Use the 32-bit intrinsic on 32-bit architectures
  uint32_t lower_source = static_cast<uint32_t>(source);
  uint32_t lower_mask = static_cast<uint32_t>(mask);

  uint32_t upper_source = static_cast<uint32_t>(source >> 32);
  uint32_t upper_mask = static_cast<uint32_t>(mask >> 32);

  // Extract lower and upper results
  uint32_t lower_result = _pext_u32(lower_source, lower_mask);
  uint32_t upper_result = _pext_u32(upper_source, upper_mask);

  // Combine results: lower result stays, upper result is shifted
  uint64_t combined_result = static_cast<uint64_t>(lower_result) |
    (static_cast<uint64_t>(upper_result) << __builtin_popcount(lower_mask));
  return combined_result;
#endif
}
} // namespace bmi2_detail

#define BMI_2D_X_MASK 0x5555555555555555
#define BMI_2D_Y_MASK 0xAAAAAAAAAAAAAAAA

template <typename morton, typename coord>
inline morton m2D_e_BMI(const coord x, const coord y)
{
  morton m = 0;
  m |= bmi2_detail::pdep(static_cast<morton>(x), static_cast<morton>(BMI_2D_X_MASK)) |
    bmi2_detail::pdep(static_cast<morton>(y), static_cast<morton>(BMI_2D_Y_MASK));
  return m;
}

template <typename morton, typename coord>
inline void m2D_d_BMI(const morton m, coord& x, coord& y)
{
  x = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_2D_X_MASK)));
  y = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_2D_Y_MASK)));
}

#define BMI_3D_X_MASK 0x9249249249249249
#define BMI_3D_Y_MASK 0x2492492492492492
#define BMI_3D_Z_MASK 0x4924924924924924

template <typename morton, typename coord>
inline morton m3D_e_BMI(const coord x, const coord y, const coord z)
{
  morton m = 0;
  m |= bmi2_detail::pdep(static_cast<morton>(x), static_cast<morton>(BMI_3D_X_MASK)) |
    bmi2_detail::pdep(static_cast<morton>(y), static_cast<morton>(BMI_3D_Y_MASK)) |
    bmi2_detail::pdep(static_cast<morton>(z), static_cast<morton>(BMI_3D_Z_MASK));
  return m;
}

template <typename morton, typename coord>
inline void m3D_d_BMI(const morton m, coord& x, coord& y, coord& z)
{
  x = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_X_MASK)));
  y = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_Y_MASK)));
  z = static_cast<coord>(bmi2_detail::pext(m, static_cast<morton>(BMI_3D_Z_MASK)));
}
}
#endif
