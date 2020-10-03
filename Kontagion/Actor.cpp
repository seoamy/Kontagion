#include "Actor.h"
#include "StudentWorld.h"
#include <string>
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//ACTOR //////////////////////////////////////////////////////////////////////
Actor::Actor(int imageID, double startX, double startY, int startHealth, StudentWorld *w, Direction dir, int depth, double size)
:GraphObject(imageID, startX, startY, dir, depth, size)
{
    m_health = startHealth;
    m_World = w;
}

Actor::~Actor()
{
}

StudentWorld* Actor::getWorld() const
{
    return m_World;
}

int Actor::getHealth() const
{
    return m_health;
}

void Actor::setHealth(int hp)
{
    m_health = hp;
}

bool Actor::isAlive() const
{
    if (getHealth() > 0)
    {
        return true;
    }
    return false;
}

void Actor::setDead()
{
    m_health = 0;
}

void Actor::takeDamage(int damageAmt)
{
    m_health -= damageAmt;
}

bool Actor::isDamageableObject()
{
    return false;
}

bool Actor::isEdible()
{
    return false;
}

bool Actor::canBlock()
{
    return false;
}

bool Actor::preventsLevelCompletion()
{
    return false;
}

//SOCRATES //////////////////////////////////////////////////////////////////////
Socrates::Socrates(StudentWorld *w)
:Actor(IID_PLAYER, SOCRATES_INIT_X, SOCRATES_INIT_Y, SOCRATES_INIT_HEALTH, w, SOCRATES_INIT_DIR, SOCRATES_INIT_DEPTH)
{
    m_sprayCharges = SOCRATES_INIT_SPRAY;
    m_flameCharges = SOCRATES_INIT_FLAME;
    m_sprayedLastTurn = false;
}

void Socrates::pickedUpFlamethrowerGoodie()
{
    m_flameCharges += 5;
}

void Socrates::pickedUpFungus()
{
    takeDamage(30);
}

int Socrates::getSprayCharges()
{
    return m_sprayCharges;
}

int Socrates::getFlameCharges()
{
    return m_flameCharges;
}

void Socrates::takeDamage(int damageAmt)
{
    Actor::takeDamage(damageAmt);
    if (!(isAlive()))
    {
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    else
    {
        getWorld()->playSound(SOUND_PLAYER_HURT);
    }
}

void Socrates::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    int c;
    if (getWorld()->getKey(c)) // use getKey(c) to continuously receive input
    {
        switch(c)
        {
            case KEY_PRESS_SPACE: // space key to spray
                if (m_sprayCharges > 0)
                {
                    getWorld()->addSprayObject(getDirection(), getX(), getY());
                    m_sprayCharges--;
                    m_sprayedLastTurn = true;
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                }
                break;
                
            case KEY_PRESS_ENTER: // enter key to use flamethrower
                if (m_flameCharges > 0)
                {
                    int dir = getDirection();
                    for (int i = 0; i < 16; i++)
                    {
                        // add 16 new flame objects with x and y coordinates to be SPRITE_WIDTH distance away from Socrates
                        double x = (getX() + SPRITE_WIDTH * cos(dir*1.0 / 360 * 2 * M_PI));
                        double y = (getY() + SPRITE_WIDTH  * sin(dir*1.0 / 360 * 2 * M_PI));
                        getWorld()->addFlameObject(dir, x, y);
                        dir += 22; // makes sure the flame objects are added around socrates
                    }
                    m_flameCharges--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                break;
                
            case KEY_PRESS_LEFT: //move Socrates counterclockwise
                setDirection(getDirection() + 5);
                moveTo(VIEW_RADIUS - VIEW_RADIUS * cos(getDirection() * M_PI / 180), VIEW_RADIUS - VIEW_RADIUS * sin(getDirection() * M_PI / 180)); // calculates new X and Y coordinates to make sure he moves in the circumference
                break;
                
            case KEY_PRESS_RIGHT: //move Socrates clockwise
                setDirection(getDirection() - 5);
                moveTo(VIEW_RADIUS - VIEW_RADIUS * cos(getDirection() * M_PI / 180), VIEW_RADIUS - VIEW_RADIUS * sin(getDirection() * M_PI / 180)); // calculates new X and Y coordinates to make sure he moves in the circumference
                break;
        }
    }
    else
    { // if spray charges is less than max spray charges and user did not spray last turn, increase the number of spray charges
        if (m_sprayCharges < SOCRATES_INIT_SPRAY && !m_sprayedLastTurn)
        {
            m_sprayCharges++;
        }
        m_sprayedLastTurn = false;
    }
}

// STILLS //////////////////////////////////////////////////////////////////////
Stills::Stills(int imageID, int dir, StudentWorld *w, double x, double y)
:Actor(imageID, x, y, STILLS_INIT_HEALTH, w, dir, STILLS_INIT_DEPTH)
{
}

void Stills::doSomething()
{
    return;
}

// DIRT //////////////////////////////////////////////////////////////////////
Dirt::Dirt(StudentWorld *w, double x, double y)
:Stills(IID_DIRT, STILLS_INIT_DIR, w, x, y)
{}

bool Dirt::isDamageableObject()
{
    return true;
}

bool Dirt::canBlock()
{
    return true;
}

// FOOD //////////////////////////////////////////////////////////////////////
Food::Food(StudentWorld *w, double x, double y)
:Stills(IID_FOOD, FOOD_INIT_DIR, w, x, y)
{}

bool Food::isEdible()
{
    return true;
}

// BACTERIAL PITS ////////////////////////////////////////////////////////////
BacterialPit::BacterialPit(StudentWorld *w, double x, double y)
//:Stills(IID_PIT, STILLS_INIT_DIR, w, x, y)
:Actor(IID_PIT, x, y, STILLS_INIT_HEALTH, w)
{
    m_regularSalmonellaCount = 5;
    m_aggressiveSalmonellaCount = 3;
    m_ecoliCount = 2;
    m_totalBacteriaCount = 10;
}

bool BacterialPit::preventsLevelCompletion()
{
    return true;
}

void BacterialPit::doSomething()
{
    if (m_totalBacteriaCount == 0)
    {
        setDead();
    }
    else
    {
        int chanceBacteriaIsReleased = randInt(1, 50); // 1/50 chance of releasing bacteria per tick
        if (chanceBacteriaIsReleased == 1)
        {
            std::string chanceOf = "";
            if (m_regularSalmonellaCount > 0) // if there are regular salmonella left, add that as an option to the string
            {
                chanceOf += "r";
            }
            if (m_aggressiveSalmonellaCount > 0) // if there are aggressive salmonella left, add that as an option to the string
            {
                chanceOf += "a";
            }
            if (m_ecoliCount > 0) // if there are ecoli left, add that as an option to the string
            {
                chanceOf += "e";
            }
            int bacteriaTypeIndex = randInt(0, int(chanceOf.size()) - 1); // choose a random index of the string of bacteria that can potentially be added
            char bacteriaType = chanceOf[bacteriaTypeIndex];
            switch(bacteriaType) // use a switch statement to see which bacteria was chosen
            {
                case 'r':
                    getWorld()->addRegularSalmonella(getX(), getY());
                    m_regularSalmonellaCount--;
                    break;
                case 'a':
                    getWorld()->addAggressiveSalmonella(getX(), getY());
                    m_aggressiveSalmonellaCount--;
                    break;
                case 'e':
                    getWorld()->addEcoli(getX(), getY());
                    m_ecoliCount--;
                    break;
            }
            m_totalBacteriaCount--;
            getWorld()->playSound(SOUND_BACTERIUM_BORN);
        }
    }
}

// BACTERIAS ////////////////////////////////////////////////////////////
Bacteria::Bacteria(int imageID, int health, StudentWorld *w, double x, double y)
:Actor(imageID, x, y, health, w, BACTERIA_INIT_DIR, BACTERIA_INIT_DEPTH)
{
    m_foodEaten = 0;
    m_movementPlanDistance = 0;
}

int Bacteria::getNumberOfFoodEaten() const
{
    return m_foodEaten;
}

int Bacteria::getMovementPlanDistance() const
{
    return m_movementPlanDistance;
}

void Bacteria::resetNumberOfFoodEaten()
{
    m_foodEaten = 0;
}

void Bacteria::incrementFood()
{
    m_foodEaten++;
}

bool Bacteria::isDamageableObject()
{
    return true;
}

// DO SOMETHING HELPER FUNCTIONS
void Bacteria::setMovementPlanDistance(int d)
{
    m_movementPlanDistance = d;
}

void Bacteria::takeDamage(int damageAmt)
{
    Actor::takeDamage(damageAmt);
    if (getHealth() <= 0)
    {
        playHurtSound();
        getWorld()->increaseScore(100);
        int chanceOfTurningIntoFood = randInt(0, 1); // bacteria has 50% chance of turning into food
        if (chanceOfTurningIntoFood == 1)
        { // add a new food object if there is no blocking actor overlap and it is within the inner circle radius of 120 pixels as
            if (distanceFromCenter(getX(), getY()) <= 120 && !(getWorld()->checkForBlockingActorOverlap(getX(), getY())))
            {
                getWorld()->bacteriaIntoFood(getX(), getY());
            }
        }
    }
    else
    {
        playDeadSound();
    }
}

bool Bacteria::preventsLevelCompletion()
{
    return true;
}

void Bacteria::playHurtSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Bacteria::playDeadSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

double Bacteria::distanceFromCenter(double x, double y)
{
    return (sqrt((x - VIEW_WIDTH/2)*(x - VIEW_WIDTH/2) + (y - VIEW_HEIGHT/2)*(y - VIEW_HEIGHT/2)));
}

void Bacteria::findFood()
{
    double angle = getWorld()->findDirectionalAngleOfFood(getX(), getY()); // get the angle of the closest food object
    if (angle == -1) // if the function returned -1, there is no food object in range so calculate a random direction to move in
    {
        int randomDirection = randInt(0, 359);
        setDirection(randomDirection);
        setMovementPlanDistance(10);
        return;
    }
    else // if a directional angle for nearby food is found, attempt to move there
    {
        double tempx = getX();
        double tempy = getY();
        attemptMove(angle, tempx, tempy);
    }
}

void Bacteria::divide()
{
    if (getNumberOfFoodEaten() == 3) // if bacteria has eaten 3 food, calculate a new x and y coordinate depending on the bacteria's x and y coordinates
    {
        double newX, newY;
        // calculate new X coordinate
        if (getX() < VIEW_WIDTH/2)
        {
            newX = getX() + SPRITE_WIDTH/2;
        }
        else if (getX() > VIEW_WIDTH/2)
        {
            newX = getX() - SPRITE_WIDTH/2;
        }
        else
        {
            newX = getX();
        }
        
        // calculate new Y coordinate
        if (getY() < VIEW_HEIGHT/2)
        {
            newY = getY() + SPRITE_WIDTH/2;
        }
        else if (getY() > VIEW_HEIGHT/2)
        {
            newY = getY() - SPRITE_WIDTH/2;
        }
        else
        {
            newY = getY();
        }
        addSpecificBacteria(newX, newY); // add this new bacteria to the world
        resetNumberOfFoodEaten();
        doSpecificMovement();
        return;
    }
}

void Bacteria::attemptMove(int dir, double& x, double& y) // doSomething() helper function
{
    bool blocked = false;
    for (int i = 1; i < 4; i++) // check if each step is valid (3 total)
    {
        x = x + 1 * cos(dir*1.0 / 360 * 2 * M_PI); // calculate new x and y coordinates if the bacteria potentially moves one pixel forward in the given direction
        y = y + 1 * sin(dir*1.0 / 360 * 2 * M_PI);
        if (getWorld()->checkForBlockingActorOverlap(x, y) || distanceFromCenter(x, y) >= VIEW_RADIUS) //  if this new x and y coordinate is out of bounds or is blocked by a dirt, then break out of this check
        {
            blocked = true;
            break;
        }
    }
    if (blocked) // if the bacteria was calculated unable to move forward 3 pixels, then calculate a random direction to move in
    {
        int randomDirection = randInt(0, 359);
        setDirection(randomDirection);
        setMovementPlanDistance(10);
    }
    else // if the bacteria is able to move forward 3 pixels, then move there
    {
        moveTo(x, y);
    }
}

void Bacteria::doSpecificMovement() // use this definition for regular and aggressive salmonella
{
    if (getMovementPlanDistance() > 0)
    {
        setMovementPlanDistance(getMovementPlanDistance() - 1); // decrement movement plan distance by one
        double xtemp = getX();
        double ytemp = getY();
        attemptMove(getDirection(), xtemp, ytemp); // attempt to move forward 3 times in the current direction
        return;
    }
    else
    {
        findFood(); // find closest food and attempt to move there
    }
}

void Bacteria::doSomething()
{
    if (!isAlive()) // step 1: if dead, don't do anything
        return;
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY())) // step 2: if overlap with socrates, then damage him and try moving
    {
        doSpecificDamageToSocrates();
        doSpecificMovement();
        return;
    }
    divide(); // step 3: add new bacteria if eaten 3 food
    if (getWorld()->eatFood(getX(), getY())) //  step 4: if food overlaps, then eat it
    {
        incrementFood();
    }
    doSpecificMovement(); // step 5/6: move according to movement plan distance or find nearest food and move towards it
}

// SALMONELLA
Salmonella::Salmonella(StudentWorld *w, double x, double y)
:Bacteria(IID_SALMONELLA, SALMONELLA_INIT_HEALTH, w, x, y)
{}

void Salmonella::doSpecificDamageToSocrates()
{
    getWorld()->getSocrates()->takeDamage(1);
}

void Salmonella::addSpecificBacteria(double x, double y)
{
    getWorld()->addRegularSalmonella(x, y);
}

// AGGRESSIVE SALMONELLA
AggressiveSalmonella::AggressiveSalmonella(StudentWorld *w, double x, double y)
:Bacteria(IID_SALMONELLA, AGGR_SALMONELLA_INIT_HEALTH, w, x, y)
{}

void AggressiveSalmonella::doSomething()
{
    if (!isAlive()) // step 1: if dead, don't do anything
        return;
    
    bool wasAggressive = false;
    int socratesX = getWorld()->getSocrates()->getX();
    int socratesY = getWorld()->getSocrates()->getY();
    if (getWorld()->distanceBetween(getX(), getY(), socratesX, socratesY) <= 72) // if the distance between current bacteria and socrates is less than or equal to 72, try to move towards it
    {
        double dY = socratesY - getY();
        double dX = socratesX - getX();
        double angle = atan2(dY,dX) * (180/M_PI); // calculates directional angle between bacteria and socrates
        double x = getX();
        double y = getY();
        bool blocked = false;
        for (int i = 1; i < 4; i++) // attempt to move in direction of angle 3 times
        {
            x = x + 1 * cos(angle*1.0 / 360 * 2 * M_PI);
            y = y + 1 * sin(angle*1.0 / 360 * 2 * M_PI);
            if (getWorld()->checkForBlockingActorOverlap(x, y) || distanceFromCenter(x, y) >= VIEW_RADIUS) // if moving forward would cause it to be out of bounds or run into a blockable object, break out of the loop
            {
                blocked = true;
                break;
            }
        }
        if (!blocked)
        {
            moveTo(x, y); // if bacteria can move forward 3 times, do it!
        }
        wasAggressive = true; // bacteria was able to do its aggressive task
    }
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY())) // if bacteria ovleraps with Socrates, deal damage to him
    {
        doSpecificDamageToSocrates();
        if (wasAggressive) // if aggressive task was done, immediately return instead of trying to move
        {
            return;
        }
        doSpecificMovement();
        return;
    }
    divide();     // step 3: duplicating if it has eaten 3 food
    if (getWorld()->eatFood(getX(), getY()))     // step 4: eat food if it overlaps
    {
        incrementFood();
    }
    if (wasAggressive) // if step 2 was done, then immediately return
    {
        return;
    }
    doSpecificMovement();  // only do step 5/6 if it did not do its aggressive task
}

void AggressiveSalmonella::addSpecificBacteria(double x, double y)
{
    getWorld()->addAggressiveSalmonella(x, y);
}

void AggressiveSalmonella::doSpecificDamageToSocrates()
{
    getWorld()->getSocrates()->takeDamage(2);
}

// ECOLI ///////////////////////////////////////////
Ecoli::Ecoli(StudentWorld *w, double x, double y)
:Bacteria(IID_ECOLI, ECOLI_INIT_HEALTH, w, x, y)
{}

void Ecoli::playHurtSound()
{
    getWorld()->playSound(SOUND_ECOLI_HURT);
}

void Ecoli::playDeadSound()
{
    getWorld()->playSound(SOUND_ECOLI_DIE);
}

void Ecoli::doSpecificDamageToSocrates()
{
    getWorld()->getSocrates()->takeDamage(4);
}

void Ecoli::addSpecificBacteria(double x, double y)
{
    getWorld()->addEcoli(x, y);
}

void Ecoli::doSpecificMovement()
{
    double socratesX = getWorld()->getSocrates()->getX();
    double socratesY = getWorld()->getSocrates()->getY();
    double angle = atan2(socratesY - getY(), socratesX - getX()) * 180/M_PI; // calculate directional angle towards socrates
    if (getWorld()->distanceBetween(getX(), getY(), socratesX, socratesY) <= 256)
    {   // if distance between socrates and bacteria is <= 256, attempt to move towards it 10 times
        for (int i = 0; i < 10; i++)
        {
            double newX = (getX() + 2 * cos(angle*1.0 / 360 * 2 * M_PI));
            double newY = (getY() + 2 * sin(angle*1.0 / 360 * 2 * M_PI));
            if (distanceFromCenter(newX, newY) >= VIEW_RADIUS || getWorld()->checkForBlockingActorOverlap(newX, newY))
            {
                angle += 10; //  if prevented from moving in the direction, increment the angle by 10, wrapping around 360
                if (angle >= 360)
                    angle -= 360;
            }
            else
            {
                moveTo(newX, newY); // if able to move, then move
                return;
            }
        }
    }
}

// GOODIES ////////////////////////////////////////////////////////////
Goodies::Goodies(int imageID, int life, StudentWorld *w, double x, double y)
:Actor(imageID, x, y, STILLS_INIT_HEALTH, w, STILLS_INIT_DIR, STILLS_INIT_DEPTH)
{
    m_lifetime = life;
}

bool Goodies::isDamageableObject()
{
    return true;
}

void Goodies::doSomething()
{
    if (!isAlive())
        return;
    
    doSpecificGoodieTasks();
    
    if (m_lifetime == 0)
    {
        setDead();
        return;
    }
    m_lifetime--;
}

// RESTORE HEALTH GOODIE
RestoreHealthGoodie::RestoreHealthGoodie(int life, StudentWorld *w, double x, double y)
:Goodies(IID_RESTORE_HEALTH_GOODIE, life, w, x, y)
{
}

void RestoreHealthGoodie::doSpecificGoodieTasks()
{
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY()))
    {
        getWorld()->increaseScore(250); //give user 250 points
        setDead(); // set goodie health to dead
        getWorld()->playSound(SOUND_GOT_GOODIE); // play sound
        getWorld()->playerPickedUpRestoreHealthGoodie();
        return;
    }
}

// FLAMETHROWER GOODIE
FlameThrowerGoodie::FlameThrowerGoodie(int life, StudentWorld *w, double x, double y)
:Goodies(IID_FLAME_THROWER_GOODIE, life, w, x, y)
{
}

void FlameThrowerGoodie::doSpecificGoodieTasks()
{
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY()))
    {
        getWorld()->increaseScore(300); //give user 300 points
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE); // play sound
        getWorld()->playerPickedUpFlamethrowerGoodie(); // give player flames
        return;
    }
}

// EXTRA LIFE GOODIE
ExtraLifeGoodie::ExtraLifeGoodie(int life, StudentWorld *w, double x, double y)
:Goodies(IID_EXTRA_LIFE_GOODIE, life, w, x, y)
{
}

void ExtraLifeGoodie::doSpecificGoodieTasks()
{
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY()))
    {
        getWorld()->increaseScore(500); //give user 500 points
        setDead(); // set goodie health to dead
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives(); // give player extra life
        return;
    }
}

// FUNGUES BAD GOODIE
Fungus::Fungus(int life, StudentWorld *w, double x, double y)
:Goodies(IID_FUNGUS, life, w, x, y)
{
}

void Fungus::doSpecificGoodieTasks()
{
    if (getWorld()->checkForSocratesOverlap(this->getX(), this->getY()))
    {
        getWorld()->increaseScore(-50); // user loses 50 points
        setDead(); // set goodie health to dead
        getWorld()->playerPickedUpFungus(); // player loses health & points
        return;
    }
}

// PROJECTILES //////////////////////////////////////////////////////////////////////
Projectile::Projectile(int imageID, int depth, int dir, int health, StudentWorld *w, double x, double y)
:Actor(imageID, x, y, health, w, dir, depth)
{
    m_movementCount = 0;
}

int Projectile::getMovementCount() const
{
    return m_movementCount;
}

void Projectile::doSomething()
{
    if (!isAlive())
        return;
    
    if (dealSpecificProjectileDamage())
    {
        setDead();
        return;
    }
    moveAngle(getDirection(), SPRITE_WIDTH); // keep moving in direction of angle
    m_movementCount += SPRITE_WIDTH; // increment movement count
    specificMovementCheck(); // if projectile reached its max movement, kill itself
}

// FLAME PROJECTILE
FlameProjectile::FlameProjectile(StudentWorld *w, int dir, double x, double y)
:Projectile(IID_FLAME, PROJECTILE_DEPTH, dir, PROJECTILE_HEALTH, w, x, y)
{}

bool FlameProjectile::dealSpecificProjectileDamage()
{
    return getWorld()->dealProjectileDamage(5, getX(), getY());
}

void FlameProjectile::specificMovementCheck()
{
    if (getMovementCount() == FLAME_TRAVEL_DIST)
    {
        setDead();
    }
}

// SPRAY PROJECTILE
SprayProjectile::SprayProjectile(StudentWorld *w, int dir, double x, double y)
:Projectile(IID_SPRAY, PROJECTILE_DEPTH, dir, PROJECTILE_HEALTH, w, x, y)
{}

bool SprayProjectile::dealSpecificProjectileDamage()
{
    return getWorld()->dealProjectileDamage(2, getX(), getY());
}

void SprayProjectile::specificMovementCheck()
{
    if (getMovementCount() == SPRAY_TRAVEL_DIST)
    {
        setDead();
    }
}
