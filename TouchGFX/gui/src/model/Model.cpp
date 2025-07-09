#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <cstdint>

Model::Model() : modelListener(0)
{

}

void Model::tick()
{

}

void Model::setCurrentLevel(uint8_t level)
{
    gameLevel = level;
}

uint8_t Model::getCurrentLevel()
{
    return gameLevel;
}
