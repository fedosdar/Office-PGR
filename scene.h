#pragma once

#include "pgr.h"
#include "object.h"
#include "render.h"
#include "data.h"

void initObjects();
void drawObjects(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);