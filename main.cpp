#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>

#include "sprites.h"
#include "maps.h"

#include "src/Minigames/Sequence.hpp"
#include "src/Minigames/Bruteforce.hpp"
#include "src/Minigames/RobotProgram.hpp"

#include "src/Level.hpp"
#include "src/Player.hpp"
#include "src/HackLog.hpp"
#include "src/ButtonMaps.h"

#include "audio/DoorHiss.h"
#include "audio/Denied.h"
#include "audio/Comp.h"
#include "audio/Unlock.h"

#include <tasui>
#include <puits_UltimateUtopia.h>

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
using HackLog::HackLogManager;


enum State{
    INTRO, EXPLORE, LIFT,
    BRUTE_FORCE, SEQUENCE, ROBOPROGRAM,
    SEQ_INTRO, ROBO_INTRO, BRUTE_INTRO,
    HACK_LOG
};

struct UIVariants{
    static constexpr unsigned standard = 0;
    static constexpr unsigned blackBG = 8;
    static constexpr unsigned halfBlackBG = 16;
    static constexpr unsigned red = 24;
    static constexpr unsigned green = 32;
};



int camX, camY, recolor = 0, tileX, tileY, oldX, oldY, introLevel = 0, zx=30, zy=60, zc = 10, zys = 1, zxs = 1, introProg = 0;
int messageX, messageY;
int logCursorX, logCursorY, cursorTimer = 0;
bool showCursor = true, showLog = false;

int buttonsPreviousState = 0;
int buttonsJustPressed = 0;

Sprite hack, dor, botField, drone, keyIcon, lock, cursorIcon;//, nano, zero;

// nano.play(nanoPhage, NanoPhage::boom);
// zero.play(zeroTitle, ZeroTitle::idle);

State state = State::INTRO;
State prevState = State::SEQ_INTRO;

bool hacking = false, doorLocked = true, hasKey = false;

MapEnum tile;
auto getTile = seqIntroEnum;
auto lastTile = seqIntroEnum;
auto lastMap = seqIntro;

char* message = "";
bool displayMessage = false;


void init(){
    camX = 12;
    camY = 12;
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    keyIcon.play(key, Key::idle);
    lock.play(lockIcon, LockIcon::idle);
    cursorIcon.play(cursor, Cursor::idle);
    
    
    Display::loadRGBPalette(miloslav);
    LevelManager::init();
    LevelManager::setMap(seqIntro, seqIntroEnum);
    PlayerManager::init();
    HackLog::HackLogManager::init();
    // NEW - Select the tileset.
    UI::setTilesetImage(puits::UltimateUtopia::tileSet);
    // NEW - Show the Tilemap, the Sprites, then the UI.
    UI::showTileMapUISprites();
    messageX = 2, messageY = 24;
}

void update(){
    // 7 == white;
    Display::setColor(7);
    
    //display global message
    if(displayMessage){
        UI::clear();
        UI::drawBox(messageX, messageY, 32, messageY + 4);
        UI::setCursorBoundingBox(messageX + 1, messageY + 1, 32 - 1, messageY + 4 - 1);
        UI::setCursor(messageX + 1, messageY + 1);
        UI::setCursorDelta(UIVariants::standard);
        UI::printText(message);
        if(Buttons::cBtn()){
            displayMessage = false;
            UI::clear();
        }
    }
    
    
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
        LevelManager::render(-camX, -camY);
        
        if(prevState != EXPLORE){
            dor.draw(-camX+48, -camY);
        }else {
            dor.draw(-camX+96, -camY+80);
            dor.draw(-camX+112, -camY+80);
        }
        
        PlayerManager::render();
        
        BruteHack::update();
        BruteHack::render();
        
        if(BruteHack::complete()){
            message = "That sentry should be inactive now.";
            displayMessage = true;
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
        LevelManager::render(-camX, -camY);
        
        if(prevState != EXPLORE){
            dor.draw(-camX+48, -camY);
        }else {
            dor.draw(-camX+96, -camY+80);
            dor.draw(-camX+112, -camY+80);
        }
        
        PlayerManager::render();
        
        // draw the hacking icon above player
        hack.draw(PlayerManager::getX(), PlayerManager::getY()-16, false, false);
        
        SeqHack::update();
        
        if(SeqHack::complete()){
            UI::clear();
            message = "The door is unlocked!";
            displayMessage = true;
            doorLocked = false;
            Sound::playSFX(Unlock, sizeof(Unlock));
            dor.play(door, Door::unlocked);
            state = prevState;
        }
        
        if(SeqHack::fail()){
            //lose
            UI::clear();
            state = prevState;
            doorLocked = true;
            displayMessage = true;
            message = "Ugh! I need to be careful. \nThe computer took my keycard!";
            hasKey = false;
            dor.play(door, Door::locked);
        }
        
        SeqHack::render();
        
        break;
        
    case ROBOPROGRAM:
        RoboHack::update();
        
        RoboHack::render();
        LevelManager::render(0, 0);

        if(RoboHack::complete()){
            hasKey = true;
            message = "I got the keycard!";
            displayMessage = true;
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
        LevelManager::render(-camX, -camY);
        dor.draw(-camX+48, -camY);
        
        oldX = camX;
        oldY = camY;
        
        PlayerManager::update(camX, camY);
        PlayerManager::render();
        
        if(Buttons::cBtn()){
            prevState = state;
            state = HACK_LOG;
            logCursorX = 0;
            logCursorY = 0;
        }
        
        if(hasKey){
            keyIcon.draw(16, 128);
        }
        
        // get current tile
        tileX = (camX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
        tileY = (camY + PlayerManager::getY() - 8 + PlayerManager::getH()) / PROJ_TILE_H;
        tile = getTile(tileX, tileY);
        
        if(LevelManager::checkDrone(tileX * PROJ_TILE_W, tileY * PROJ_TILE_H)){
            camX = -60;
            camY = -70;
            message = "Arg!\nThe sentry returned me to the exit.";
            displayMessage = true;
        }
        
        if( tile == Sequ) {
            if( Buttons::aBtn() ){
                if((introLevel != 0 && hasKey) || introLevel == 0){
                    Sound::playSFX(Comp, sizeof(Comp));
                    SeqHack::init(60);
                    SeqHack::shuffle(6);
                    prevState = state;
                    state = State::SEQUENCE;
                }else{
                    Sound::playSFX(Denied, sizeof(Denied));
                    message = "I need a keycard to access\nthis terminal.";
                    displayMessage = true;
                }
            }
        }
        if( tile == Robo ){
            if( Buttons::aBtn() ){
                UI::clear();
                displayMessage = false;
                Sound::playSFX(Comp, sizeof(Comp));
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
                Sound::playSFX(Comp, sizeof(Comp));
                BruteHack::init(5);
                prevState = state;
                state = State::BRUTE_FORCE;
            }
        }

        if( tile == Collide ){
            camX = oldX;
            camY = oldY;
        }
        
        if( tile == Door ){
            if(dor.animation == Door::locked){
                camX = oldX;
                camY = oldY;
                Sound::playSFX(Denied, sizeof(Denied));
                message = "The door is locked! Where is the control system?";
                displayMessage = true;
            } else if(dor.animation == Door::unlocked){
                Sound::playSFX(DoorHiss, sizeof(DoorHiss));
                dor.play(door, Door::open);
            }
        }
        tileY = (camY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
        tile = getTile(tileX, tileY);
        
        if(tile==Door){
            introLevel++;
            displayMessage = false;
            UI::clear();
            camX = -60;
            camY = -70;
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
                camX = 10;
                camY = 10;
            }
            
            dor.play(door, Door::locked);
            doorLocked = true;
        }
        
        break;
    case EXPLORE:
        LevelManager::render(-camX, -camY);
        
        dor.draw(-camX+96, -camY+80);
        dor.draw(-camX+112, -camY+80);
        
        oldX = camX;
        oldY = camY;
        
        PlayerManager::update(camX, camY);
        PlayerManager::render();
        
        if(Buttons::cBtn()){
            prevState = state;
            state = HACK_LOG;
            logCursorX = 0;
            logCursorY = 0;
        }
        
        if(hasKey){
            keyIcon.draw(16, 128);
        }
        
        tileX = (camX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
        tileY = (camY + PlayerManager::getY() - 8 + PlayerManager::getH()) / PROJ_TILE_H;
        tile = getTile(tileX, tileY);
        
        if(LevelManager::checkDrone(tileX * PROJ_TILE_W, tileY * PROJ_TILE_H)){
            camX = 10;
            camY = 10;
            message = "Arg!\nThe sentry returned me to the exit.";
            displayMessage = true;
        }
        
        if( tile == Sequ) {
            if( Buttons::aBtn() ){
                if(hasKey){
                    Sound::playSFX(Comp, sizeof(Comp));
                    SeqHack::init(40);
                    SeqHack::shuffle(10);
                    prevState = state;
                    state = State::SEQUENCE;
                }else {
                    Sound::playSFX(Denied, sizeof(Denied));
                    message = "I need a keycard to access\nthis terminal.";
                    displayMessage = true;
                }
            }
        }
        if( tile == Brute){
            if(Buttons::aBtn()){
                Sound::playSFX(Comp, sizeof(Comp));
                BruteHack::init(5);
                prevState = state;
                state = State::BRUTE_FORCE;
            }
        }
        if( tile == Robo ){
            if( Buttons::aBtn() ){
                UI::clear();
                displayMessage = false;
                Sound::playSFX(Comp, sizeof(Comp));
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
            camX = oldX;
            camY = oldY;
        }
        
        if( tile == Door ){
            if(dor.animation == Door::locked){
                camX = oldX;
                camY = oldY;
                Sound::playSFX(Denied, sizeof(Denied));
                message = "The door is locked! Where is the control system?";
                displayMessage = true;
            } else if(dor.animation == Door::unlocked){
                Sound::playSFX(DoorHiss, sizeof(DoorHiss));
                dor.play(door, Door::open);
            }
        }
        
        if( tile == Lift ){
            displayMessage = false;
            UI::clear();
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
            camX = 10;
            camY = 10;
            state = State::EXPLORE;
        }
    break;
    case HACK_LOG:
        UI::clear();
        UI::showTileMapUISprites();
        UI::drawBox(1, 1, 32, 24);
        UI::setCursorBoundingBox(2, 2, 32 - 1, 24 - 1);
        UI::setCursor(2, 2);
        UI::setCursorDelta(UIVariants::standard);
        UI::printString("Hack Log: ");
        
        if(!showLog){
            for(int i = 0; i < 7; ++i){
                for(int j = 0; j < 5; ++j){
                    HackLogManager::render(i, j);
                }
            }
        }
        
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        if(buttonsJustPressed == B_RIGHT){
            if(logCursorX < 6) logCursorX++;
        }
        if(buttonsJustPressed == B_LEFT){
            if(logCursorX > 0) logCursorX--;
        }
        if(buttonsJustPressed == B_DOWN){
            if(logCursorY < 4) logCursorY++;
        }
        if(buttonsJustPressed == B_UP){
            if(logCursorY > 0) logCursorY--;
        }
            
        cursorTimer++;
        if(cursorTimer > 15) {
            showCursor = !showCursor;
            cursorTimer = 0;
        }
        if(showCursor) cursorIcon.draw(17 + logCursorX * 24, 30 + logCursorY * 24);
        
        if(buttonsJustPressed == B_A){
            if(HackLogManager::checkUnlocked(logCursorX, logCursorY)){
                showLog = true;
            }
        }
        
        if(!showLog && buttonsJustPressed == B_B){
            UI::clear();
            state = prevState;
        }
        
        if(showLog){
            if(buttonsJustPressed == B_B) showLog = false;
            UI::drawBox(4, 4, 28, 20);
            UI::setCursorBoundingBox(6, 6, 28 - 1, 20 - 1);
            UI::setCursor(6, 6);
            UI::setCursorDelta(UIVariants::standard);
            UI::printText(HackLogManager::getLog(logCursorX, logCursorY));
        }
        
    break;
    }
}