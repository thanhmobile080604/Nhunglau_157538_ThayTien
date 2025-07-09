#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::updateLevel()
{
	currentLevel++;
	if(currentLevel > MAX_LEVEL) {
		currentLevel = 1; // Reset về level 1
	}

	// Lưu vào Model
	presenter->setGameLevel(currentLevel);

	updateLevelDisplay();
}

void Screen1View::updateLevelDisplay()
{
    Unicode::snprintf(textArea1Buffer, 20, "%d", currentLevel);
    textArea1.setWildcard(textArea1Buffer);
    textArea1.invalidate();
}
