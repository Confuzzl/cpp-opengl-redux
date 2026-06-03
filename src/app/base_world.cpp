#include "app/base_world.h"

#include <glm/gtc/matrix_transform.hpp>

BaseWorld::BaseWorld(const glm::vec3 light) : cam{}, light{light} {}