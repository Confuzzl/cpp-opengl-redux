#include "color.h"

#include "util.h"

namespace colors {
Color random() {
  return {random_int(0u, 255u), random_int(0u, 255u), random_int(0u, 255u)};
}
} // namespace colors