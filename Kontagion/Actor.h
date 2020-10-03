#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// Constants for classes:
// Constants for Socrates
const int SOCRATES_INIT_HEALTH = 100;
const int SOCRATES_INIT_X = 0;
const int SOCRATES_INIT_Y = 128;
const int SOCRATES_INIT_DIR = 0;
const int SOCRATES_INIT_DEPTH = 0;
const int SOCRATES_INIT_SPRAY = 20;
const int SOCRATES_INIT_FLAME = 5;

// Consants for Dirt, Food
const int STILLS_INIT_HEALTH = 1;
const int STILLS_INIT_DIR = 0;
const int STILLS_INIT_DEPTH = 1;
// stills must be no more than 120 pixels from the center of the petri dish
const int STILLS_POS_LIMIT = 120;
const int FOOD_INIT_DIR = 90;

// Constants for Bacteria
const int BACTERIA_INIT_DIR = 90;
const int BACTERIA_INIT_DEPTH = 0;
const int SALMONELLA_INIT_HEALTH = 4;
const int AGGR_SALMONELLA_INIT_HEALTH = 10;
const int ECOLI_INIT_HEALTH = 5;

// Constants for Projectiles
const int PROJECTILE_DEPTH = 1;
const int PROJECTILE_HEALTH = 1;
const int PROJECTILE_DIR = 0;
const int FLAME_TRAVEL_DIST = 32;
const int SPRAY_TRAVEL_DIST = 112;

class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int startHealth,
          StudentWorld* w, Direction dir = 0, int depth = 0, double size = 1.0);    // constructor for every basic actor
    virtual ~Actor();                       // virtual destructor
    StudentWorld* getWorld() const;         // every actor needs to be able to access the current world it exists in
    int getHealth() const;                  // every actor needs to be able to return its current health level
    void setHealth(int hp);                 // used for when Socrates needs to set his health back to his original state
    bool isAlive() const;                   // every actor needs to be able to know if its alive
    void setDead();
    
    // virtual functions
    virtual void takeDamage(int damageAmt);
    virtual bool isEdible();
    virtual bool isDamageableObject();
    virtual bool canBlock();
    virtual bool preventsLevelCompletion();
    
    // pure virtual functions
    virtual void doSomething() = 0;
    
private:
    int m_health;
    StudentWorld *m_World;
};

class Socrates: public Actor
{
public:
    Socrates(StudentWorld *w);
    void pickedUpFlamethrowerGoodie();
    void pickedUpFungus();
    int getSprayCharges();
    int getFlameCharges();
    virtual void takeDamage(int damageAmt);
    virtual void doSomething();
private:
    int m_sprayCharges;
    int m_flameCharges;
    bool m_sprayedLastTurn;
    double m_angle;
};

// STILLS //////////////////////////////////////////////////////////////////////
class Stills: public Actor
{
public:
    Stills(int imageID, int dir, StudentWorld *w, double x, double y);
    virtual void doSomething();
};

class Dirt: public Stills
{
public:
    Dirt(StudentWorld *w, double x, double y);
    virtual bool isDamageableObject();
    virtual bool canBlock();
};

class Food: public Stills
{
public:
    Food(StudentWorld *w, double x, double y);
    virtual bool isEdible();
};

// BACTERIAL PIT //////////////////////////////////////////////////////////////////////
class BacterialPit: public Actor
{
public:
    BacterialPit(StudentWorld *w, double x, double y);
    virtual void doSomething();
    virtual bool preventsLevelCompletion();
private:
    int m_regularSalmonellaCount;
    int m_aggressiveSalmonellaCount;
    int m_ecoliCount;
    int m_totalBacteriaCount;
    
};

// BACTERIAS //////////////////////////////////////////////////////////////////////
class Bacteria: public Actor
{
public:
    Bacteria(int imageID, int health, StudentWorld *w, double x, double y);
    int getNumberOfFoodEaten() const;
    int getMovementPlanDistance() const;
    void resetNumberOfFoodEaten();
    void incrementFood();
    virtual bool isDamageableObject();
    virtual void takeDamage(int damageAmt);
    virtual bool preventsLevelCompletion();
    virtual void playHurtSound();
    virtual void playDeadSound();
    virtual void doSpecificDamageToSocrates() = 0;
    virtual void addSpecificBacteria(double x, double y) = 0;
    
    //doSomething() helper functions
    double distanceFromCenter(double x, double y);
    void setMovementPlanDistance(int d);
    void findFood();
    void divide();
    void attemptMove(int dir, double& x, double& y);
    virtual void doSpecificMovement();
    //
    virtual void doSomething();
private:
    int m_foodEaten;
    int m_movementPlanDistance;
};

class Salmonella: public Bacteria
{
public:
    Salmonella(StudentWorld *w, double x, double y);
    virtual void addSpecificBacteria(double x, double y);
    virtual void doSpecificDamageToSocrates();
};

class AggressiveSalmonella: public Bacteria
{
public:
    AggressiveSalmonella(StudentWorld *w, double x, double y);
    virtual void doSomething();
    virtual void addSpecificBacteria(double x, double y);
    virtual void doSpecificDamageToSocrates();
};

class Ecoli: public Bacteria
{
public:
    Ecoli(StudentWorld *w, double x, double y);
    virtual void playHurtSound();
    virtual void playDeadSound();
    virtual void doSpecificDamageToSocrates();
    virtual void addSpecificBacteria(double x, double y);
    virtual void doSpecificMovement();
};

// GOODIES //////////////////////////////////////////////////////////////////////
class Goodies: public Actor
{
public:
    Goodies(int imageID, int life, StudentWorld *w, double x, double y);
    virtual void doSomething();
    virtual bool isDamageableObject();
    virtual void doSpecificGoodieTasks() = 0;
private:
    int m_lifetime;
};

class RestoreHealthGoodie: public Goodies
{
public:
    RestoreHealthGoodie(int life, StudentWorld *w, double x, double y);
    virtual void doSpecificGoodieTasks();
};

class FlameThrowerGoodie: public Goodies
{
public:
    FlameThrowerGoodie(int life, StudentWorld *w, double x, double y);
    virtual void doSpecificGoodieTasks();
};

class ExtraLifeGoodie: public Goodies
{
public:
    ExtraLifeGoodie(int life, StudentWorld *w, double x, double y);
    virtual void doSpecificGoodieTasks();
};

class Fungus: public Goodies
{
public:
    Fungus(int life, StudentWorld *w, double x, double y);
    virtual void doSpecificGoodieTasks();
};

// PROJECTILES //////////////////////////////////////////////////////////////////////
class Projectile: public Actor
{
public:
    Projectile(int imageID, int depth, int dir, int health, StudentWorld *w, double x, double y);
    virtual void doSomething();
    int getMovementCount() const;
    virtual bool dealSpecificProjectileDamage() = 0;
    virtual void specificMovementCheck() = 0;
private:
    int m_movementCount;
    
};

// Flame
class FlameProjectile: public Projectile
{
public:
    FlameProjectile(StudentWorld *w, int dir, double x, double y);
    virtual bool dealSpecificProjectileDamage();
    virtual void specificMovementCheck();
};

class SprayProjectile: public Projectile
{
public:
    SprayProjectile(StudentWorld *w, int dir, double x, double y);
    virtual bool dealSpecificProjectileDamage();
    virtual void specificMovementCheck();
};

#endif // ACTOR_H_
