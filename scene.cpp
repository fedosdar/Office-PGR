#include "scene.h"
#include "pgr.h"
#include "controls.h"
#include "render.h"
#include "camera.h"
#include "object.h"
#include "data.h"

extern struct GameState gameState;

static unsigned int id = 0;
Skybox		 skybox;
StaticObject armchair	= StaticObject(++id, ARMCHAIR, glm::vec3(0.736f, 0.0f, -0.85f), 0.23f, glm::radians(5.9f));
StaticObject box		= StaticObject(++id, BOX, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f);
StaticObject chair		= StaticObject(++id, CHAIR, glm::vec3(0.325f, 0.0f, 0.8f), 0.09f, glm::radians(41.5f));
StaticObject ground		= StaticObject(++id, GROUND, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f);
StaticObject lama		= StaticObject(++id, LAMA, glm::vec3(-0.4f, 0.0f, -0.15f), 0.000145f, glm::radians(61.8f));
StaticObject monitor	= StaticObject(++id, MONITOR, glm::vec3(0.6f, 0.39f, 1.34f), 0.076f, glm::radians(197.0f), true, false);
StaticObject paperplane	= StaticObject(++id, PAPERPLANE, glm::vec3(-0.5f, 2.0f, 0.0f), 0.000720f, glm::radians(-17.2f));
StaticObject printer	= StaticObject(++id, PRINTER, glm::vec3(0.2f, 0.41f, 1.32f), 0.082f, glm::radians(172.0f));
StaticObject table		= StaticObject(++id, TABLE, glm::vec3(0.47f, 0.15f, 1.3f), 25.7f, glm::radians(-90.0f));
StaticObject tissueBox	= StaticObject(++id, TISSUEBOX, glm::vec3(0.75f, 0.365f, 1.24f), 0.214464f, (float)glm::radians(-70.6928));
StaticObject window		= StaticObject(++id, WINDOW,	 glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, false, true);
StaticObject pepper		= StaticObject(++id, "none", glm::vec3(0.736f, 0.27f, -0.85f), 1.0f, 0.0f);

void initObjects() {
	skybox.initGeometry();

	armchair.initGeometry();
	box.initGeometry();
	chair.initGeometry();
	ground.initGeometry();
	lama.initGeometry();
	monitor.initGeometry();
	paperplane.initGeometry();
	printer.initGeometry();
	table.initGeometry();
	tissueBox.initGeometry();
	window.initGeometry();
	pepper.initHardcodedGeometry();
}

void drawObjects(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	skybox.draw(viewMatrix, projectionMatrix);

	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	armchair.draw(viewMatrix, projectionMatrix);
	box.draw(viewMatrix, projectionMatrix);
	chair.draw(viewMatrix, projectionMatrix);
	ground.draw(viewMatrix, projectionMatrix);
	lama.draw(viewMatrix, projectionMatrix);
	monitor.draw(viewMatrix, projectionMatrix);
	paperplane.draw(viewMatrix, projectionMatrix);
	printer.draw(viewMatrix, projectionMatrix);
	table.draw(viewMatrix, projectionMatrix);
	tissueBox.draw(viewMatrix, projectionMatrix);
	window.draw(viewMatrix, projectionMatrix);
	pepper.draw(viewMatrix, projectionMatrix);

	glDisable(GL_STENCIL_TEST);
	CHECK_GL_ERROR();
}