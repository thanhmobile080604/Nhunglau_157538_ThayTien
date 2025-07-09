#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

Screen2Presenter::Screen2Presenter(Screen2View& v)
    : view(v)
{

}

void Screen2Presenter::activate()
{

}

void Screen2Presenter::deactivate()
{

}

uint8_t Screen2Presenter::getGameLevel()
{
    return model->getCurrentLevel();
}


void Screen2Presenter::goToScreen3View()
{
    // Chuyển sang Screen3
    static_cast<FrontendApplication*>(Application::getInstance())->gotoScreen3ScreenNoTransition();
}
