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
#include "HackLog.hpp"

#include "DoorHiss.h"
#include "Denied.h"

#include <tasui>
#include <puits_UltimateUtopia.h>

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
    using Pokitto::Sound;
    using Pokitto::UI;
    using Sequence::SeqHack;
    using Bruteforce::BruteHack;
    using RobotProgram::RoboHack;
    using Level::LevelManager;
    using Player::PlayerManager;
    
    Core::begin();
    Display::loadRGBPalette(miloslav);
    
    LevelManager::init();
    LevelManager::setMap(seqIntro, seqIntroEnum);
    
    PlayerManager::init();
    
    HackLog::HackLogManager::init();
    
    int cameraX = 12, cameraY = 12, recolor = 0, tileX, tileY, oldX, oldY, introLevel = 0, zx=30, zy=60, zc = 10, zys = 1, zxs = 1, introProg = 0;

    Sprite hack, dor, botField, drone, keyIcon;//, nano, zero;
    
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    keyIcon.play(key, Key::idle);
    
    // nano.play(nanoPhage, NanoPhage::boom);
    // zero.play(zeroTitle, ZeroTitle::idle);
    
    State state = State::INTRO;
    State prevState = State::SEQ_INTRO;
    
    bool hacking = false, doorLocked = true, hasKey = false;
    
    MapEnum tile;
    auto getTile = seqIntroEnum;
    auto lastTile = seqIntroEnum;
    auto lastMap = seqIntro;
    
    
      // NEW - Select the tileset.
    UI::setTilesetImage(puits::UltimateUtopia::tileSet);
    // NEW - Show the Tilemap, the Sprites, then the UI.
    UI::showTileMapSpritesUI();
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        Display::setColor(7);//white
        switch(state){
        case INTRO:
            UI::drawGauge(5, 25, 16, introProg, 100);
            // nano.draw(LCDWIDTH/2-nano.getFrameWidth()/2, 10);
            // nano.onEndAnimation = [](Sprite *nano){nano->play(nanoPhage, NanoPhage::idle); }; 
            // zero.draw(zx, zy);
            // zc--;
            // if(zc == 0) {
            //     zc = 10;
            //     zx+=zxs;
            //     zy+=zys;
            //     if(zy > 65 || zy < 60) zys = -zys;
            //     if(zx > 35 || zx < 30) zxs = -zxs;
            // }
            
            if( Buttons::cBtn() ){
                introProg++;
                if(introProg == 100){
                    UI::clear();
                    state = State::SEQ_INTRO;
                }
            }
            
            Display::setColor(7);
            Display::print(30, 140, "C to start.");
            
            break;
        case BRUTE_FORCE:
            LevelManager::render(-cameraX, -cameraY);
            
            if(prevState != EXPLORE){
                dor.draw(-cameraX+48, -cameraY);
            }else {
                dor.draw(-cameraX+96, -cameraY+80);
                dor.draw(-cameraX+112, -cameraY+80);
            }
            
            PlayerManager::render();
            
            BruteHack::update();
            BruteHack::render();
            
            if(BruteHack::complete()){
                state = prevState;
                LevelManager::setDroneState(false);
            }
            
            if(BruteHack::fail()){
                //lose
                state = prevState;
                LevelManager::setDroneState(true);
            }
            
            break;
        case SEQUENCE:
            LevelManager::render(-cameraX, -cameraY);
            
            if(prevState != EXPLORE){
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
                hasKey = true;
                if(prevState != EXPLORE){
                    LevelManager::setMap(lastMap, lastTile);
                    getTile = lastTile;
                    HackLog::HackLogManager::unlockLog(0);
                }else {
                    LevelManager::setMap();
                    getTile = lastTile;
                }
                state = prevState;
            }
        
            break;
        case SEQ_INTRO:
        case ROBO_INTRO:
        case BRUTE_INTRO:
            LevelManager::render(-cameraX, -cameraY);
            dor.draw(-cameraX+48, -cameraY);
            
            PlayerManager::render();
            
            oldX = cameraX;
            oldY = cameraY;
            
            PlayerManager::update(cameraX, cameraY);
            
            if(hasKey){
                keyIcon.draw(16, 128);
            }
            
            // get current tile
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(LevelManager::checkDrone(tileX * PROJ_TILE_W, tileY * PROJ_TILE_H)){
                cameraX = -60;
                cameraY = -70;
            }
            
            if( tile == Sequ) {
                if( Buttons::aBtn() ){
                    if((introLevel != 0 && hasKey) || introLevel == 0){
                        SeqHack::init();
                        SeqHack::shuffle(4);
                        prevState = state;
                        state = State::SEQUENCE;
                    }else{
                        Display::print("I need a keycard to access this terminal.");   
                    }
                }
            }
            if( tile == Robo ){
                if( Buttons::aBtn() ){
                    RoboHack::init(13);
                    RoboHack::setIntro(false);
                    LevelManager::setMap(minibotfield, minibotfieldEnum);
                    lastTile = getTile;
                    getTile = minibotfieldEnum;
                    prevState = state;
                    state = State::ROBOPROGRAM;
                }
            }
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
                    Sound::playSFX(Denied, sizeof(Denied));
                    Display::print("The door is locked! Where is the control system?");
                } else if(dor.animation == Door::unlocked){
                    Sound::playSFX(DoorHiss, sizeof(DoorHiss));
                    dor.play(door, Door::open);
                }
            }
            tileY = (cameraY + PlayerManager::getY() + 30 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                introLevel++;
                cameraX = -60;
                cameraY = -70;
                hasKey = false;
                if(introLevel == 1){
                    lastMap = roboIntro;
                    LevelManager::setMap(roboIntro, roboIntroEnum);
                    getTile = roboIntroEnum;
                    state = State::ROBO_INTRO;
                }
                if(introLevel == 2){
                    lastMap = bruteIntro;
                    LevelManager::setMap(bruteIntro, bruteIntroEnum);
                    LevelManager::setDroneState(true);
                    getTile = bruteIntroEnum;
                    state = State::BRUTE_INTRO;
                    
                }
                if(introLevel == 3){
                    lastMap = lobby;
                    LevelManager::setMap(lobby, lobbyEnum);
                    LevelManager::setDroneState(true);
                    getTile = lobbyEnum;
                    state = State::EXPLORE;
                    cameraX = 10;
                    cameraY = 10;
                }
                
                dor.play(door, Door::locked);
                doorLocked = true;
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
            
            if(hasKey){
                keyIcon.draw(16, 128);
            }
            
            tileX = (cameraX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(LevelManager::checkDrone(tileX * PROJ_TILE_W, tileY * PROJ_TILE_H)){
                cameraX = 10;
                cameraY = 10;
            }
            
            if( tile == Sequ) {
                if( Buttons::aBtn() ){
                    if(hasKey){
                        SeqHack::init();
                        SeqHack::shuffle(8);
                        prevState = state;
                        state = State::SEQUENCE;
                    }else {
                        Display::print("I need a keycard to access this terminal.");
                    }
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
                    RoboHack::init(13);
                    RoboHack::setIntro(false);
                    LevelManager::setMap(minibotfield, minibotfieldEnum);
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
                hasKey = false;
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
                LevelManager::setDroneState(true);
                cameraX = 10;
                cameraY = 10;
                state = State::EXPLORE;
            }
        break;
        }
    }
    return 0;
}
