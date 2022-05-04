#include "controls.h"
#include "pgr.h"
#include "camera.h"
#include <iostream>

struct GameState gameState;
Camera camera;

void passiveMouseMotionCallback(int mouseX, int mouseY) {
	if (camera.freeCamera) {
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		float x = static_cast<float>(mouseX) - gameState.windowWidth / 2;
		float y = static_cast<float>(mouseY) - gameState.windowHeight / 2;

		// Rotating camera
		camera.changeAngles(x, y);
		camera.update();
		glutPostRedisplay();
	}
}

void activeMouseCallback(int button, int state, int mouseX, int mouseY) {
	if (state == GLUT_DOWN) {
		unsigned char idClicked = 0;
		glReadPixels(mouseX, gameState.windowHeight - mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &idClicked);

		std::cout << "ID " << (int)idClicked << std::endl;

		if (idClicked == 2)
			gameState.day = !gameState.day;
		if (idClicked == 5)
			gameState.reflector = !gameState.reflector;
		if (idClicked == 6)
			gameState.pointLight = !gameState.pointLight;
	}
}

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {
	switch (keyPressed) {
	case 27: // Escape - exits the application
		glutLeaveMainLoop();
		break;
	case 'c': // Free camera
		camera.freeCamera = !camera.freeCamera;
		break;
	default:
		std::cout << "Unrecognized key pressed." << std::endl;
	}
}


void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {
	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		camera.changePosition();
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		camera.changePosition();
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		camera.changePosition();
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		camera.changePosition();
		break;
	case GLUT_KEY_F1:
		camera.setView1();
		break;
	case GLUT_KEY_F2:
		camera.setView2();
		break;
	default:
		std::cout << "Unrecognized special key pressed." << std::endl;
	}
}


	void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) {
	switch (specKeyReleased) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	default:
		std::cout << "Unrecognized special key released." << std::endl;
	}
}

void timerCallback(int) {
	// Updating game time (converted to seconds)
	gameState.currentTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);
	//gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);

	glutTimerFunc(33, timerCallback, 0);

	glutPostRedisplay();
}