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

//TODO: move the EXPLORE and HALLWAY states into the Level class.
enum State{
    INTRO, 
    BRUTE_FORCE, SEQUENCE, ROBOPROGRAM,
    EXPLORE, HALLWAY
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
    
    int cameraX = 12, cameraY = 12, recolor = 0, tileX, tileY, oldX, oldY;

    Sprite hack, dor, botField;
    
    botField.play(minibotField, MinibotField::idle);
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    
    State state = State::EXPLORE;
    State prevState = State::EXPLORE;
    
    bool hacking = false, doorLocked = true;
    MapEnum tile;
    auto getTile = seqIntroEnum;
    auto lastGetTile = getTile;
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        
        switch(state){
        case INTRO:
            
            if( Buttons::cBtn() ){
                state = State::EXPLORE;
            }
            
            Display::setColor(7);
            Display::print("C to start.");
            
            break;
        case BRUTE_FORCE:
            // tilemap.draw(-cameraX, -cameraY);
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
            PlayerManager::render();
            
            BruteHack::update();
            BruteHack::render();
            break;
        case SEQUENCE:
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
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
            LevelManager::render(-RoboHack::getRobotX(), -RoboHack::getRobotY());

            if(RoboHack::complete()){
                doorLocked = false;
                dor.play(door, Door::unlocked);
                LevelManager::setMap(roboIntro);
                getTile = roboIntroEnum;
                state = prevState;
            }
        
            break;
        case EXPLORE:
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
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                LevelManager::setMap(roboIntro);
                cameraX = -58;
                cameraY = -70;
                getTile = roboIntroEnum;
                dor.play(door, Door::locked);
                doorLocked = true;
                state = State::HALLWAY;
            }
            
            Display::setColor(7);
            break;
        case HALLWAY:
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
        break;
        }
    }
    return 0;
}
