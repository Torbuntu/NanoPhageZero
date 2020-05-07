#include "global.h"
#include <miloslav.h>

#include "audio/DoorHiss.h"
#include "audio/Denied.h"
#include "audio/Comp.h"
#include "audio/Unlock.h"

enum State{
    INTRO, EXPLORE, LIFT,
    BRUTE_FORCE, SEQUENCE, ROBOPROGRAM,
    SEQ_INTRO, ROBO_INTRO, BRUTE_INTRO,
    HACK_LOG
};

State state = State::INTRO;
State prevState = State::SEQ_INTRO;

int camX, camY, tileX, tileY, oldX, oldY, introLevel = 0, zx=30, zy=60, zc = 10, zys = 1, zxs = 1, introProg = 0;
int messageX, messageY;

Sprite hack, dor, keyIcon;//, nano, zero;

bool hacking = false, doorLocked = true, hasKey = false;

MapEnum tile;
auto getTile = seqIntroEnum;
auto lastTile = seqIntroEnum;
auto lastMap = seqIntro;

char* message = "";
bool displayMessage = false;

void checkRobo(){
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
}

void checkBrute(){
    if( tile == Brute){
        hack.draw(PlayerManager::getX(), PlayerManager::getY()-16, false, false);
        if(Buttons::aBtn()){
            Sound::playSFX(Comp, sizeof(Comp));
            BruteHack::init(5);
            prevState = state;
            state = State::BRUTE_FORCE;
        }
    }
}

void checkSequ(){
    if( tile == Sequ) {
        hack.draw(PlayerManager::getX(), PlayerManager::getY()-16, false, false);
        if( Buttons::aBtn() ){
            if((introLevel != 0 && hasKey) || introLevel == 0){
                Sound::playSFX(Comp, sizeof(Comp));
                if(introLevel < 3) {
                    SeqHack::init(60);
                    SeqHack::shuffle(6);
                }else{
                    SeqHack::init(40);
                    SeqHack::shuffle(10);
                }
                prevState = state;
                state = State::SEQUENCE;
            }else{
                Sound::playSFX(Denied, sizeof(Denied));
                message = "I need a keycard to access\nthis terminal.";
                if(state == ROBO_INTRO) HackLogManager::unlockLog(2);
                displayMessage = true;
            }
        }
    }
}

void checkDroneActive(){
    if(LevelManager::checkDrone(tileX * PROJ_TILE_W, tileY * PROJ_TILE_H)){
        if(state != EXPLORE){
            camX = -60;
            camY = -70;
        }else{
            camX = 10;
            camY = 10;
        }
        message = "Arg!\nThe sentry returned me to the exit.";
        if(state == BRUTE_INTRO) HackLogManager::unlockLog(3);
        displayMessage = true;
    }
}

void checkDoor(){
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
}

void checkCollide(){
    if( tile == Collide ){
        camX = oldX;
        camY = oldY;
    }
}

void openHackLog(){
    if(Buttons::cBtn()){
        prevState = state;
        state = HACK_LOG;
        HackLogManager::setShouldEnd(false);
    }
}

void primaryUpdate(){
    oldX = camX;
    oldY = camY;
    
    PlayerManager::update(camX, camY);
    PlayerManager::render();
    
    openHackLog();
    
    if(hasKey){
        keyIcon.draw(16, 128);
    }
    
    tileX = (camX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
    tileY = (camY + PlayerManager::getY() - 8 + PlayerManager::getH()) / PROJ_TILE_H;
    tile = getTile(tileX, tileY);
    
    if(tile == Log){
        switch(state){
            case SEQ_INTRO:
                HackLogManager::unlockLog(4);
                break;
        }
        
    }
    
    checkDroneActive();
    checkSequ();
    checkBrute();
    checkRobo();
    checkCollide();
    checkDoor();
}

void init(){
    camX = 12;
    camY = 12;
    
    zx = 0;
    zy = 160;
    
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    keyIcon.play(key, Key::idle);
    
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
    
    Sound::playMusicStream("music/npz-t.raw", 0);
}

void update(){
    
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
        Sound::playMusicStream();
        UI::drawGauge(8, 28, 17, introProg, 100);

        if( Buttons::cBtn() ){
            introProg++;
            if(introProg == 100){
                UI::clear();
                state = State::SEQ_INTRO;
                Sound::pauseMusicStream();
            }
        }
        // 7 == white;
        // 207 == Green
        
        Display::setColor(135);
        Display::print(zx, 80, "Nano");
        
        Display::setColor(207);
        Display::print(zy, 80, "Phage");
        
        if(zx < (LCDWIDTH - 64)/2)zx++;
        if(zy > LCDWIDTH/2 + 4)zy--;
        if(zx == (LCDWIDTH - 64)/2 && zy == LCDWIDTH/2 + 4){
            Display::setColor(5);
            Display::print((LCDWIDTH - 32)/2, 90, "ZERO");
        }
        
        Display::setColor(7);
        Display::print((LCDWIDTH - strlen("C to start")*8)/2, 140, "C to start.");
        
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
            if(prevState == SEQ_INTRO){
                HackLogManager::unlockLog(1);
            }
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
        
        primaryUpdate();
        
        tileY = (camY + PlayerManager::getY() + 8 + PlayerManager::getH()) / PROJ_TILE_H;
        tile = getTile(tileX, tileY);
        
        if(tile==Door){
            introLevel++;
            displayMessage = false;
            UI::clear();
            camX = -60;
            camY = -70;
            hasKey = false;
            PlayerManager::setDir(3);//face south
            if(introLevel == 1){
                HackLogManager::unlockLog(5);
                lastMap = roboIntro;
                LevelManager::setMap(roboIntro, roboIntroEnum);
                getTile = roboIntroEnum;
                state = State::ROBO_INTRO;
            }
            if(introLevel == 2){
                HackLogManager::unlockLog(6);
                lastMap = bruteIntro;
                LevelManager::setMap(bruteIntro, bruteIntroEnum);
                LevelManager::setDroneState(true);
                getTile = bruteIntroEnum;
                state = State::BRUTE_INTRO;
                
            }
            if(introLevel == 3){
                HackLogManager::unlockLog(7);
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
        
        primaryUpdate();
        
        if( tile == Lift ){
            displayMessage = false;
            UI::clear();
            hasKey = false;
            prevState = state;
            state = State::LIFT;
            PlayerManager::setDir(3);//face south
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
                case 2:
                    getTile = lvl2Enum;
                    break;
                case 3:
                
                    break;
                    
                case 4: //final boss
                
                    break;
            }
            doorLocked = true;
            dor.play(door, Door::locked);
            
            LevelManager::setMap();
            LevelManager::setDroneState(true);
            camX = 10;
            camY = 10;
            PlayerManager::setDir(3);//face south
            state = State::EXPLORE;
        }
    break;
    case HACK_LOG:
        HackLogManager::update();
        HackLogManager::render();
        
        if(HackLogManager::shouldEnd()){
            UI::clear();
            UI::showTileMapSpritesUI();
            state = prevState;
        }

    break;
    }
}
