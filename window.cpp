#include "window.h"
#include "pgr.h"
#include "controls.h"
#include "render.h"
#include "scene.h"
#include "camera.h"

extern Camera					camera;
extern struct SkyboxShader		skyboxShader;
extern struct RegularShader		regularShader;
extern struct GameState			gameState;

void drawWindowContents() {
	glUseProgram(regularShader.program);
	glUniform1f(regularShader.timeLocation, gameState.currentTime);

	CHECK_GL_ERROR();

	glUseProgram(0);

	// Drawing all objects
	drawObjects(camera.viewMatrix(), camera.projectionMatrix());

	CHECK_GL_ERROR();
}

void displayCallback() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

	drawWindowContents();

	glutSwapBuffers();
}

void reshapeCallback(int newWidth, int newHeight) {
	gameState.windowWidth = newWidth;
	gameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

void initApplication() {
	// Setting default background colour
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initializing shaders
	initShaderPrograms();

	// Initializing objects and their models
	initObjects();
}

void finalizeApplication(void) {
	cleanupShaderPrograms();
}

// 
void resetWindow() {

}