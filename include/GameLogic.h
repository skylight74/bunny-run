#include "Model.h"

class GameLogic {
public:
  // ... other methods and members ...

  void animateBunny(Model &bunnyModel, float deltaTime);

  // ... other methods and members ...
private:
  float hopAmplitude = 0.5f;
};
