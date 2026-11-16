#pragma once

/**
 * @file    types.hpp
 * @brief   ...
 * @author  ...
 */

#include <fpm/fixed.hpp>

namespace aegis
{
// =============================================================================
// Core type aliases
// =============================================================================

/**
 * @brief Engine-wide general purpose fixed-point type (Q20.12 signed).
 */
using q20_12_t = fpm::fixed<std::int32_t, std::int64_t, 12>;

// =============================================================================
// Engine-wide default capacity limits
// =============================================================================
namespace EngineLimits
{
// ...
} // namespace EngineLimits
} // namespace aegis

// alias for aegis
namespace ae = aegis;
