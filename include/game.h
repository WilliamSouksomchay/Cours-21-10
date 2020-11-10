#pragma once
#include <SFML/Graphics/Sprite.hpp>



#include "contact_listener.h"
#include "platform.h"
#include "player_character.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "box2d/b2_world.h"

class Game
{
public:
    Game();
    void Init();
    void Loop();

    b2Body* CreateBody(const b2BodyDef& bodyDef);
    void BeginContact(UserDataType userData, UserDataType userData1);
    void EndContact(UserDataType userData, UserDataType userData1);
    const float pixelToMeter = 100.0f;
private:
    //Physics time step at 50 Hz
    const float fixedTimestep_ = 0.02f;
    float fixedTimer_ = 0.0f;

    sf::RenderWindow window_;
    sf::Texture wall_;
    sf::Sprite sprite_;
    b2World world_{b2Vec2(0.0f, 9.81f)};

    PlayerCharacter player_;
    Platform platform_;

    MyContactListener contactListener_;
};