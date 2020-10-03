#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Actor* getSocrates();
    
    // level completion
    void increaseLevelCompletion();
    void decreaseLevelCompletion();
    
    // player picking up goodies
    void playerPickedUpRestoreHealthGoodie();
    void playerPickedUpFlamethrowerGoodie();
    void playerPickedUpFungus();
    
    // adding actor objects
    void addBacterialPits();
    void addFood();
    void addDirtPiles();
    void addSprayObject(int dir, double x, double y);
    void addFlameObject(int dir, double x, double y);
    void addRegularSalmonella(double x, double y);
    void addAggressiveSalmonella(double x, double y);
    void addEcoli(double x, double y);
    void bacteriaIntoFood(double x1, double y1);
    
    // dealing damage to actors
    bool dealProjectileDamage(int dmgAmt, double x, double y);
    bool eatFood(double x, double y);
    
    // helper functions
    double findDirectionalAngleOfFood(double x, double y);
    double distanceBetween(double x1, double y1, double x2, double y2);
    bool checkForOverlap(double x, double y);
    bool checkForBlockingActorOverlap(double x, double y);
    bool checkForSocratesOverlap(double x, double y);
    void generateRandomInnerCircleCoordinates(double& x, double& y);
    void generateRandomCircumferenceCoordinates(double& x, double& y);
    bool isWithinOverlapDistance(double x1, double y1, double x2, double y2);
    bool isWithinBacteriaOverlapDistance(double x1, double y1, double x2, double y2);

private:
    std::list<Actor*> allActors;
    Socrates* m_socrates;
    int m_numActorsPreventingLevelCompletion;
};

#endif // STUDENTWORLD_H_
