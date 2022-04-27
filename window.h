#pragma once

#include "data.h"
#include "pgr.h"

void drawWindowContents();

void displayCallback();

void reshapeCallback(int newWidth, int newHeight);

void initApplication();

void finalizeApplication(void);

void resetWindow();