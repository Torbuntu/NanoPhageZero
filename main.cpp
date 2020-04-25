#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

#include "Minigames/Sequence.hpp"
#include "Minigames/Bruteforce.hpp"
#include "Minigames/RobotProgram.hpp"

#include "Level.hpp"
#include "Player.hpp"

//TODO: move the  states into the Level class.
enum State{
    INTRO, EXPLORE, LIFT,
    BRUTE_FORCE, SEQUENCE, ROBOPROGRAM,
    SEQ_INTRO, ROBO_INTRO, BRUTE_INTRO
};

int main(){
    using Pokitto::Core;
    using Pokitto::Display;
    using Pokitto::Buttons;
    using Sequence::SeqHack;
    using Bruteforce::BruteHack;
    using RobotProgram::RoboHack;
    using Level::LevelManager;
    using Player::PlayerManager;
    
    Core::begin();
    Display::loadRGBPalette(miloslav);
    
    LevelManager::init();
    LevelManager::setMap(seqIntro);
    
    PlayerManager::init();
    
    int cameraX = 12, cameraY = 12, recolor = 0, tileX, tileY, oldX, oldY, droneX = 12, droneY = 12, d;

    Sprite hack, dor, botField, drone;
    
    botField.play(minibotField, MinibotField::idle);
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    drone.play(securityDrone, SecurityDrone::idle);
    
    State state = State::SEQ_INTRO;
    State prevState = State::SEQ_INTRO;
    
    bool hacking = false, doorLocked = true;
    
    MapEnum tile;
    auto getTile = seqIntroEnum;
    auto lastTile = seqIntroEnum;
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        Display::setColor(7);//white
        switch(state){
        case INTRO:
            
            if( Buttons::cBtn() ){
                state = State::SEQ_INTRO;
            }
            
            Display::setColor(7);
            Display::print("C to start.");
            
            break;
        case BRUTE_FORCE:
            // tilemap.draw(-cameraX, -cameraY);
            LevelManager::render(-cameraX, -cameraY);
            
            if(prevState == State::BRUTE_INTRO){
                dor.draw(-cameraX+48, -cameraY);
            }else {
                dor.draw(-cameraX+96, -cameraY+80);
                dor.draw(-cameraX+112, -cameraY+80);
            }
            
            PlayerManager::render();
            
            BruteHack::update();
            BruteHack::render();
            
            if(BruteHack::complete()){
                doorLocked = false;
                dor.play(door, Door::unlocked);
                state = prevState;
            }
            
            if(BruteHack::fail()){
                //lose
                state = prevState;
                doorLocked = true;
                dor.play(door, Door::locked);
            }
            
            break;
        case SEQUENCE:
            LevelManager::render(-cameraX, -cameraY);
            
            if(prevState == State::SEQ_INTRO){
                dor.draw(-cameraX+48, -cameraY);
            }else {
                dor.draw(-cameraX+96, -cameraY+80);
                dor.draw(-cameraX+112, -cameraY+80);
            }
            
            PlayerManager::render();
            
            // draw the hacking icon above player
            hack.draw(PlayerManager::getX(), PlayerManager::getY()-16, false, false);
            
            SeqHack::update();
            
            if(SeqHack::fail()){
                //lose
                state = prevState;
                doorLocked = true;
                dor.play(door, Door::locked);
            }

            if(SeqHack::complete()){
                doorLocked = false;
                dor.play(door, Door::unlocked);
                state = prevState;
            }
            
            SeqHack::render();
            
            break;
            
        case ROBOPROGRAM:
            RoboHack::update();
            
            RoboHack::render();
            LevelManager::render(0, 0);

            if(RoboHack::complete()){
                doorLocked = false;
                dor.play(door, Door::unlocked);
                if(prevState == ROBO_INTRO){
                    LevelManager::setMap(roboIntro);
                    getTile = lastTile;
                }else {
                    LevelManager::setMap();
                    getTile = lastTile;
                }
                state = prevState;
            }
        
            break;
        case SEQ_INTRO:
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
            PlayerManager::render();
            
            oldX = cameraX;
            oldY = cameraY;
            PlayerManager::update(cameraX, cameraY);
            
            // get current tile
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if( tile == Sequ) {
                if( Buttons::aBtn() ){
                    SeqHack::init();
                    SeqHack::shuffle(4);
                    prevState = state;
                    state = State::SEQUENCE;
                }
            }
            if( tile == Brute){
                
            }
            if( tile == Robo ){
                if( Buttons::aBtn() ){
                    RoboHack::init(4);
                    LevelManager::setMap(minibotfield);
                    getTile = minibotfieldEnum;
                    prevState = state;
                    state = State::ROBOPROGRAM;
                }
            }
    
            if( tile == Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
            
            if( tile == Door ){
                if(dor.animation == Door::locked){
                    cameraX = oldX;
                    cameraY = oldY;
                    Display::print("The door is locked! Where is the control system?");
                } else if(dor.animation == Door::unlocked){
                    dor.play(door, Door::open);
                }
            }
            tileY = (cameraY + PlayerManager::getY() + 32 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                LevelManager::setMap(roboIntro);
                cameraX = -58;
                cameraY = -70;
                getTile = roboIntroEnum;
                dor.play(door, Door::locked);
                doorLocked = true;
                state = State::ROBO_INTRO;
            }
            
            break;
        case ROBO_INTRO:
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
            botField.draw(-cameraX+88, -cameraY+88);
            
            PlayerManager::render();
            
            oldX = cameraX;
            oldY = cameraY;
            
            PlayerManager::update(cameraX, cameraY);
            
            // get current tile
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);

            if( tile == Robo ){
                if( Buttons::aBtn() ){
                    RoboHack::init(13);
                    LevelManager::setMap(minibotfield);
                    lastTile = getTile;
                    getTile = minibotfieldEnum;
                    prevState = state;
                    state = State::ROBOPROGRAM;
                }
            }
            
            if( tile == Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
            
            if( tile == Door ){
                if(dor.animation == Door::locked){
                    cameraX = oldX;
                    cameraY = oldY;
                    Display::print("The door is locked! Where is the control system?");
                } else if(dor.animation == Door::unlocked){
                    dor.play(door, Door::open);
                }
            }
            
            
            tileY = (cameraY + PlayerManager::getY() + 32 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                LevelManager::setMap(bruteIntro);
                cameraX = -58;
                cameraY = -70;
                getTile = bruteIntroEnum;
                dor.play(door, Door::locked);
                doorLocked = true;
                state = State::BRUTE_INTRO;
            }
            
        break;
        case BRUTE_INTRO:
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
            
            drone.draw(droneX - cameraX, droneY - cameraY);
            
            PlayerManager::render();
            
            oldX = cameraX;
            oldY = cameraY;
            
            PlayerManager::update(cameraX, cameraY);
            
            // get current tile
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);

            if( tile == Brute){
                if(Buttons::aBtn()){
                    BruteHack::init(5);
                    prevState = state;
                    state = State::BRUTE_FORCE;
                }
            }
            
            if( tile == Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
            
            if( tile == Door ){
                if(dor.animation == Door::locked){
                    cameraX = oldX;
                    cameraY = oldY;
                    Display::print("The door is locked! Where is the control system?");
                } else if(dor.animation == Door::unlocked){
                    dor.play(door, Door::open);
                }
            }
            
            if(doorLocked){
                d = rand()%4;
                switch(d){
                    case 0:
                        if(cameraX + droneX + 1 < 100)droneX++;
                        break;
                    case 1:
                        if(cameraX + droneX - 1 > -100)droneX--;
                        break;
                    case 2:
                        if(cameraY + droneY + 1 < 80)droneY++;
                        break;
                    case 3:
                        if(cameraY + droneY - 1 > 80)droneY--;
                        break;
                }
            }
            
            tileY = (cameraY + PlayerManager::getY() + 32 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                LevelManager::setMap(lobby);
                cameraX = 10;
                cameraY = 10;
                getTile = lobbyEnum;
                dor.play(door, Door::locked);
                doorLocked = true;
                state = State::EXPLORE;
            }
        
        break;
        case EXPLORE:
            LevelManager::render(-cameraX, -cameraY);
            
            dor.draw(-cameraX+96, -cameraY+80);
            dor.draw(-cameraX+112, -cameraY+80);
            
            oldX = cameraX;
            oldY = cameraY;
            
            PlayerManager::update(cameraX, cameraY);
            PlayerManager::render();
            
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if( tile == Sequ) {
                if( Buttons::aBtn() ){
                    SeqHack::init();
                    SeqHack::shuffle(8);
                    prevState = state;
                    state = State::SEQUENCE;
                }
            }
            if( tile == Brute){
                if(Buttons::aBtn()){
                    BruteHack::init(5);
                    prevState = state;
                    state = State::BRUTE_FORCE;
                }
            }
            if( tile == Robo ){
                if( Buttons::aBtn() ){
                    RoboHack::init(8);
                    LevelManager::setMap(minibotfield);
                    lastTile = getTile;
                    getTile = minibotfieldEnum;
                    prevState = state;
                    state = State::ROBOPROGRAM;
                }
            }
    
            if( tile == Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
            
            if( tile == Door ){
                if(dor.animation == Door::locked){
                    cameraX = oldX;
                    cameraY = oldY;
                    Display::print("The door is locked! Where is the control system?");
                } else if(dor.animation == Door::unlocked){
                    dor.play(door, Door::open);
                }
            }
            
            if( tile == Lift ){
                prevState = state;
                state = State::LIFT;
            }
        break;
        case LIFT:
            Display::print("Press C to continue.");
            if(Buttons::cBtn()){
                LevelManager::addLvl();
                switch(LevelManager::getLvl()){
                    case 0:
                        getTile = lobbyEnum;
                        break;
                    case 1:
                        getTile = lvl1Enum;
                        break;
                }
                doorLocked = true;
                dor.play(door, Door::locked);
                
                LevelManager::setMap();
                cameraX = 10;
                cameraY = 10;
                state = State::EXPLORE;
            }
        break;
        }
    }
    return 0;
}
