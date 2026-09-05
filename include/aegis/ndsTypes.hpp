#pragma once

#include <fpm/fixed.hpp>

/**
 * @file ndsTypes.hpp
 * @brief Aegis Engine — Fixed point for NDS, types matching those used
 *        by libnds. Used since the NDS lacks an fpu / having
 *        type saftey when using fixed types.
 *        Use q20_12_t for standard usage.
 */

namespace aegis
{

/**
 * @brief Engine-wide General purpose fixed-point type (Q20.12 signed).
 *
 * Replaces floats since NDS lacks an fpu.
 */
using q20_12_t = fpm::fixed<std::int32_t, std::int64_t, 12>;

/**
 * @brief vertex coordinate type (Q4.12 signed), matches libnds v16.
 *
 * Used by trig_lut.h for sinLerp/cosLerp/asinLerp/acosLerp's ratio.
 */
using q4_12_t = fpm::fixed<std::int16_t, std::int32_t, 12>;

/**
 * @brief GPU normal/light direction type (Q0.10 signed), matches libnds v10.
 */
using q0_10_t = fpm::fixed<std::int16_t, std::int32_t, 10>;

/**
 * @brief GPU texture coordinate type (Q12.4 signed), matches libnds t16.
 */
using q12_4_t = fpm::fixed<std::int16_t, std::int32_t, 4>;

/**
 * @brief GPU depth value type (Q12.3 unsigned), matches libnds fixed12d3.
 *
 * Unsigned since depth is never negative.
 * type-safety typedef for conversion sites (glClearDepth, glCutoffDepth).
 */
using q12_3_t = fpm::fixed<std::uint16_t, std::uint32_t, 3>;

/**
 * @brief Raw cyclic angle type, matches libnds's trig_lut.h angle convention.
 */
using angle16_t = std::int16_t;
} // namespace aegis

namespace ae = aegis;
