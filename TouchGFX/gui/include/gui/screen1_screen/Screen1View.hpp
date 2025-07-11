#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void updateLevel();
    void updateLevelDisplay();

protected:
    uint8_t currentLevel=1;
    const uint8_t MAX_LEVEL=4;
};

#endif // SCREEN1VIEW_HPP
