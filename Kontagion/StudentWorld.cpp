#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{ // add socrates, dirt, food, and bacterial pits to initialize each level
    m_numActorsPreventingLevelCompletion = getLevel(); // number of bacterial pits corresponds with level number
    m_socrates = new Socrates(this);
    addDirtPiles();
    addBacterialPits();
    addFood();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_socrates; // delete player and set him equal to nullptr incase he is ever accessed again
    m_socrates = nullptr;
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while (itr != allActors.end()) // iterate through all actors and delete them all
    {
        Actor* kill = *itr;
        itr = allActors.erase(itr);
        delete kill;
    }
}

Actor* StudentWorld::getSocrates()
{
    return m_socrates;
}

void StudentWorld::increaseLevelCompletion()
{
    m_numActorsPreventingLevelCompletion++;
}

void StudentWorld::decreaseLevelCompletion()
{
    m_numActorsPreventingLevelCompletion--;
}

void StudentWorld::playerPickedUpFungus()
{
    m_socrates->pickedUpFungus();
}

void StudentWorld::playerPickedUpFlamethrowerGoodie()
{
    m_socrates->pickedUpFlamethrowerGoodie();
}

void StudentWorld::playerPickedUpRestoreHealthGoodie()
{
    m_socrates->setHealth(SOCRATES_INIT_HEALTH);
}

void StudentWorld::addBacterialPits()
{
    int numPits = getLevel();
    for (int i = 0; i < numPits; i++)
    {
        double x1, y1;
        generateRandomInnerCircleCoordinates(x1, y1);
        while (checkForOverlap(x1, y1)) // continue generating random coordinates if it overlaps with anything
        {
            generateRandomInnerCircleCoordinates(x1, y1);
        }
        allActors.push_back(new BacterialPit(this, x1, y1)); // add new bacterial pit
    }
}

void StudentWorld::addFood()
{
    int numFood = min(5 * getLevel(), 25);
    for (int i = 0; i < numFood; i++)
    {
        double x1, y1;
        generateRandomInnerCircleCoordinates(x1, y1);
        while (checkForOverlap(x1, y1)) // continue generating random coordinates fi it overlaps with anything
        {
            generateRandomInnerCircleCoordinates(x1, y1);
        }
        allActors.push_back(new Food(this, x1, y1)); //  add new food object
    }
}

void StudentWorld::addDirtPiles()
{
    int numDirtPiles = max(180 - 20 * getLevel(), 20);
    for (int i = 0; i < numDirtPiles; i++)
    {
        double x1, y1;
        generateRandomInnerCircleCoordinates(x1, y1); // don't have to check for overlap because dirt objects can overlap with each other
        allActors.push_back(new Dirt(this, x1, y1));
    }
}

void StudentWorld::addSprayObject(int dir, double x, double y)
{
    allActors.push_back(new SprayProjectile(this, dir, x, y));
}

void StudentWorld::addFlameObject(int dir, double x, double y)
{
    allActors.push_back(new FlameProjectile(this, dir, x, y));
}

void StudentWorld::addRegularSalmonella(double x, double y)
{
    increaseLevelCompletion(); // called everytime a bacteria is added because it prevents level from being completed
    allActors.push_back(new Salmonella(this, x, y));
}

void StudentWorld::addAggressiveSalmonella(double x, double y)
{
    increaseLevelCompletion();
    allActors.push_back(new AggressiveSalmonella(this, x, y));
}

void StudentWorld::addEcoli(double x, double y)
{
    increaseLevelCompletion();
    allActors.push_back(new Ecoli(this, x, y));
}

void StudentWorld::bacteriaIntoFood(double x1, double y1)
{
    allActors.push_front(new Food(this, x1, y1)); //  add new food object at coordinates of dead bacteria
}

bool StudentWorld::dealProjectileDamage(int dmgAmt, double x, double y)
{
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while(itr != allActors.end())
    {
        if (isWithinOverlapDistance((*itr)->getX(), (*itr)->getY(), x, y) && (*itr)->isDamageableObject()) // find first actor that is damageable and within range of the flame projectile and deal damage to it
        {
            (*itr)->takeDamage(dmgAmt);
            return true; // immediately return because projectile can only damage one actor
        }
        itr++;
    }
    return false;
}

bool StudentWorld::eatFood(double x, double y)
{
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while(itr != allActors.end())
    {
        if (isWithinOverlapDistance((*itr)->getX(), (*itr)->getY(), x, y) && (*itr)->isEdible()) // if food overlaps with current coordinates, then eat it by setting it to dead
        {
            (*itr)->setDead();
            return true;
        }
        itr++;
    }
    return false;
}

double StudentWorld::findDirectionalAngleOfFood(double x, double y)
{
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    int smallestDistance = 99999; // dummy value
    Actor* foodToBeFound = nullptr;
    while(itr != allActors.end())
    {
        int distanceFromFoodToBacteria = distanceBetween(x, y, (*itr)->getX(), (*itr)->getY()); //  calculate distance between coordinates and the current actor
        if ((*itr)->isEdible() && distanceFromFoodToBacteria <= 128)
        {
            if (distanceFromFoodToBacteria < smallestDistance) // make sure to find the smallest distance to food
            {
                smallestDistance = distanceFromFoodToBacteria;
                foodToBeFound = *itr;
            }
        }
        itr++;
    }
    if (foodToBeFound == nullptr)
    {
        return -1; // if no food is found within range, then return -1
    }
    else
    {
        double dY = abs(foodToBeFound->getY() - y);
        double dX = abs(foodToBeFound->getX() - x);
        double angle = atan2(dY,dX) * (180/M_PI); // calculate angle between coordinates and the closest food
        return angle;
    }
}

int StudentWorld::move()
{   // give each actor a chance to doSomething()
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while(itr != allActors.end())
    {
        if ((*itr)->isAlive())
        {
            (*itr)->doSomething();
            itr++;
        }
        if (!(m_socrates->isAlive()))
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        else
        {
            m_socrates->doSomething();
        }
        if (m_numActorsPreventingLevelCompletion == 0)
        {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    itr = allActors.begin();
    while (itr != allActors.end())
    {
        if (!(*itr)->isAlive())     // remove dead actors after each tick
        {
            if ((*itr)->preventsLevelCompletion())
                m_numActorsPreventingLevelCompletion--;
            Actor* dead = *itr;
            itr = allActors.erase(itr);
            delete dead;
        }
        else
        {
            itr++;
        }
    }
    // potentially add new actors into the game
    int chanceFungus = max(510 - getLevel() * 10, 200); // chance of adding fungus
    int fungus = randInt(0, chanceFungus-1);
    if (fungus == 0)
    {
        double x1, y1;
        generateRandomCircumferenceCoordinates(x1, y1);
        int fungusLifeTime = max(randInt(0, 300 - 10 * getLevel() - 1), 50);
        allActors.push_back(new Fungus(fungusLifeTime, this, x1, y1));
    }

    int chanceGoodie = max(510 - getLevel() * 10, 250); // chance of adding a goodie
    int goodie = randInt(0, chanceGoodie - 1);
    if (goodie == 0)
    {
        double x1, y1;
        generateRandomCircumferenceCoordinates(x1, y1);
        int goodieLife = max(randInt(0, 300 - 10 * getLevel() - 1), 50);
        int goodieType = randInt(1, 100);
        if (goodieType <= 60) // 60% chance of adding a restore health goodie
        {
            allActors.push_back(new RestoreHealthGoodie(goodieLife, this, x1, y1));
        }
        else if (goodieType <= 90) // 30% chance of adding a flamethrower goodie
        {
            allActors.push_back(new FlameThrowerGoodie(goodieLife, this, x1, y1));
        }
        else if (goodieType <= 100) // 10% chance of adding an extra life goodie
        {
            allActors.push_back(new ExtraLifeGoodie(goodieLife, this, x1, y1));
        }
    }
    // update game status text
    ostringstream oss;
    oss.fill('0');
    oss << "Score: ";
    oss << setw(6) << getScore();
    oss.fill(' ');
    oss << "  Level:";
    oss << setw(3) << getLevel();
    oss << "  Lives:";
    oss << setw(2) << getLives();
    oss << "  Health:";
    oss << setw(4) << m_socrates->getHealth();
    oss << "  Sprays:";
    oss << setw(3) << m_socrates->getSprayCharges();
    oss << "  Flames:";
    oss << setw(3) << m_socrates->getFlameCharges();
    setGameStatText(oss.str());
    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::checkForSocratesOverlap(double x, double y)
{
    double socratesX = m_socrates->getX();
    double socratesY = m_socrates->getY();
    if (isWithinOverlapDistance(socratesX, socratesY, x, y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool StudentWorld::checkForOverlap(double x, double y)
{
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while (itr != allActors.end())
    {
        double actorX = (*itr)->getX();
        double actorY = (*itr)->getY();
        if (isWithinOverlapDistance(actorX, actorY, x, y))
        {
            return true;
        }
        itr++;
    }
    return false;
}

bool StudentWorld::checkForBlockingActorOverlap(double x, double y)
{
    list<Actor*>::iterator itr;
    itr = allActors.begin();
    while (itr != allActors.end())
    {
        double actorX = (*itr)->getX();
        double actorY = (*itr)->getY();
        if (isWithinBacteriaOverlapDistance(actorX, actorY, x, y) && (*itr)->canBlock())
        {
            return true;
        }
        itr++;
    }
    return false;
}

void StudentWorld::generateRandomCircumferenceCoordinates(double& x, double& y)
{
    double theta = randInt(0, 360); //  calculate random angle
    x = VIEW_RADIUS - VIEW_RADIUS * cos(theta * M_PI/180);
    y = VIEW_RADIUS - VIEW_RADIUS * sin(theta * M_PI/180);
}


void StudentWorld::generateRandomInnerCircleCoordinates(double& x, double& y)
{
    x = randInt((VIEW_RADIUS - STILLS_POS_LIMIT), (VIEW_RADIUS + STILLS_POS_LIMIT)); //  choose random x coordinate within 120 pixels of center
    y = randInt(0, sqrt(STILLS_POS_LIMIT*STILLS_POS_LIMIT - (VIEW_RADIUS-x)*(VIEW_RADIUS-x))); //  choose random y coordinate that keeps the coordinate within 120 pixels of center
    int r = randInt(0,1); //  randomly choose if y should be negative or positive relative to the center of the circle
    if (r == 0)
    {
        y *= -1;
    }
    else
    {
        y *= 1;
    }
    y+=VIEW_RADIUS;
}

double StudentWorld::distanceBetween(double x1, double y1, double x2, double y2)
{
    return (sqrt((x2-x1)*(x2-x1) + (y2 - y1)*(y2 - y1)));
}

bool StudentWorld::isWithinOverlapDistance(double x1, double y1, double x2, double y2)
{
    if (distanceBetween(x1, y1, x2, y2) <= SPRITE_WIDTH)
    {
        return true;
    }
    return false;
}

bool StudentWorld::isWithinBacteriaOverlapDistance(double x1, double y1, double x2, double y2)
{
    if (distanceBetween(x1, y1, x2, y2) <= (SPRITE_WIDTH/2))
    {
        return true;
    }
    return false;
}
