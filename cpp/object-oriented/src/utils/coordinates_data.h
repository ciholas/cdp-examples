// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__COORDINATES_DATA__H_
#define _UTILS__COORDINATES_DATA__H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Contains the X, Y, and Z coordinates from a separately defined location.
// Total size is 12 bytes.
// Note that each field is transmitted in little endian.
typedef struct PACKED {
  int32_t x_in_mm;  // X-Coordiante: The x-coordinate from the origin (4 bytes).
  int32_t y_in_mm;  // Y-Coordiante: The y-coordinate from the origin (4 bytes).
  int32_t z_in_mm;  // Z-Coordiante: The z-coordinate from the origin (4 bytes).
} coordinates_t;

#ifdef __cplusplus
}
#endif

#endif // _UTILS__COORDINATES_DATA__H_
