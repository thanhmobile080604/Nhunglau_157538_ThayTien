#ifndef MODEL_HPP
#define MODEL_HPP
#include <cstdint>
class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    // Level management;
    void setCurrentLevel(uint8_t level);
	uint8_t getCurrentLevel();
protected:
    ModelListener* modelListener;
    uint8_t gameLevel;
};

#endif // MODEL_HPP
