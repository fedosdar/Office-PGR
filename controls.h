#pragma once

#include "data.h"
#include "pgr.h";

// Key map
enum {
	KEY_LEFT_ARROW, 
	KEY_RIGHT_ARROW, 
	KEY_UP_ARROW, 
	KEY_DOWN_ARROW, 
	KEY_COUNT
};

struct GameState {

	int windowWidth;
	int windowHeight;

	bool keyMap[KEY_COUNT];
	
	float currentTime;
	//float elapsedTime;

	bool day;
	bool reflector;
	bool pointLight;
};

void passiveMouseMotionCallback(int mouseX, int mouseY);
void activeMouseCallback(int button, int state, int mouseX, int mouseY);

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY);

void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY);
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY);

void timerCallback(int);