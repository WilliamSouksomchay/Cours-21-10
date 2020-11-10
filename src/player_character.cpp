
#define _USE_MATH_DEFINES
#include <cmath>
#include "player_character.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <sstream>
#include "game.h"
#include <box2d/box2d.h>


PlayerCharacter::PlayerCharacter(Game& game) : game_(game)
{
}

void PlayerCharacter::Init()
{
    if(!idleTexture_.loadFromFile("data/sprites/images/arret/arret.png"))
    {
        std::cerr << "Could not load hero idle texture!\n";
    }
    for(int i = 0; i < 3; i++)
    {
        std::ostringstream oss;
        oss << "data/sprites/images/marche/marche" << (i + 1) << ".png";
        walkTexture_[i].loadFromFile(oss.str());
    }
    jumpTexture_.loadFromFile("data/sprites/images/saut/saut.png");   
    UpdateSpriteTexture(idleTexture_);
    sprite_.setScale(playerScale_, playerScale_);


    b2BodyDef bodyDef;
    bodyDef.fixedRotation = true;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(1.0f, 1.0f);
    body_ = game_.CreateBody(bodyDef);

    const auto textureSize = idleTexture_.getSize()-borderMargin_*2u;
    b2Vec2 boxSize;
    boxSize.Set(
        textureSize.x / game_.pixelToMeter / 2.0f * playerScale_, 
        textureSize.y / game_.pixelToMeter / 2.0f * playerScale_);
    b2PolygonShape playerBox;
    playerBox.SetAsBox(boxSize.x, boxSize.y);

    b2FixtureDef playerFixtureDef;
    playerFixtureDef.shape = &playerBox;
    playerFixtureDef.density = 1.0f;
    playerFixtureDef.friction = 0.0f;
    playerFixtureDef.userData = &playerBoxData;
    body_->CreateFixture(&playerFixtureDef);

    b2PolygonShape playerFootBox;
    b2Vec2 footCenter;
    footCenter.Set(0.0f, boxSize.y);
    playerFootBox.SetAsBox(boxSize.x, 0.2f, footCenter, 0.0f);

    b2FixtureDef playerFootFixtureDef;
    playerFootFixtureDef.isSensor = true;
    playerFootFixtureDef.shape = &playerFootBox;
    playerFootFixtureDef.userData = &playerFootData;
    body_->CreateFixture(&playerFootFixtureDef);




    
}

void PlayerCharacter::Update(float dt)
{
    float dx = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        dx -= 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        dx += 1.0f;
    }
    auto velocity = body_->GetLinearVelocity();
    velocity.Set(dx * speed_, velocity.y);
    body_->SetLinearVelocity(velocity);

    switch(state_)
    {
    case State::IDLE:
    {
        if(footContact_ == 0)
        {
            ChangeState(State::JUMP);
        }
        if(std::abs(dx) > deadZone_)
        {
            ChangeState(State::WALK);
        }
        break;
    }
    case State::WALK:
    {
        if (footContact_ == 0)
        {
            ChangeState(State::JUMP);
        }
        //Flip when facing right and going left
        if(facingRight_ && dx < 0.0f)
        {
            Flip();
        }
        //Flip when facing left and going right
        if(!facingRight_ && dx > 0.0f)
        {
            Flip();
        }

        //Walk Anim
        walkAnimTimer_ += dt;
        if(walkAnimTimer_ > walkAnimPeriod_)
        {
            walkAnimIndex_++;
            if(walkAnimIndex_ >= walkTexture_.size())
            {
                walkAnimIndex_ = 0;
            }

            UpdateSpriteTexture(walkTexture_[walkAnimIndex_]);
            walkAnimTimer_ -= walkAnimPeriod_;
        }

        if(std::abs(dx) < deadZone_)
        {
            ChangeState(State::IDLE);
        }
        break;
    }
    case State::JUMP:
    {
        if(footContact_ > 0)
        {
            ChangeState(State::IDLE);
        }
        break;
    }
    default: ;
    }
    const auto& b2Position = body_->GetPosition();
    sprite_.setPosition(sf::Vector2f(b2Position.x * game_.pixelToMeter, b2Position.y * game_.pixelToMeter));
    const auto b2rotation = body_->GetAngle();
    sprite_.setRotation(b2rotation / M_PI * 180.0f);
    /*
    auto position = sprite_.getPosition();
    position.x += dx * speed_ * dt;
    sprite_.setPosition(position);
    */
    //std::cout << "Body position: " << body_->GetPosition().x << ", " << body_->GetPosition().y << ")\n";
}

void PlayerCharacter::Render(sf::RenderWindow& window)
{
    window.draw(sprite_);
}

sf::Vector2<float> PlayerCharacter::GetPosition() const
{
    return sprite_.getPosition();
}

void PlayerCharacter::BeginContactGround()
{
    footContact_++;
}

void PlayerCharacter::EndContactGround()
{
    footContact_--;
}

void PlayerCharacter::ChangeState(State state)
{
    switch (state)
    {
    case State::IDLE:
    {
        UpdateSpriteTexture(idleTexture_);
        break;
    }
    case State::WALK:
    {
        UpdateSpriteTexture(walkTexture_[0]);
        break;
    }
    case State::JUMP:
    {
        UpdateSpriteTexture(jumpTexture_);
        break;
    }
    default: break;
    }
    state_ = state;
}

void PlayerCharacter::UpdateSpriteTexture(const sf::Texture& texture)
{
    sprite_.setTexture(texture);
    const auto textureSize = texture.getSize();
    sprite_.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
}

void PlayerCharacter::Flip()
{
    facingRight_ = !facingRight_;
    auto scale = sprite_.getScale();
    scale.x = -scale.x;
    sprite_.setScale(scale);
}
