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
    HACK_LOG, 
    FINAL_BOSS, END_GAME
};

State state = INTRO;
State prevState = SEQ_INTRO;

int camX, camY, tileX, tileY, oldX, oldY, introLevel = 0, zx=30, zy=60, zc = 10, zys = 1, zxs = 1, introProg = 0, lvlSelect = 0;
int messageX, messageY;
int lrTop = 72, lrBottom=136, lrRight=144, lrLeft = 64;

Sprite hack, dor, keyIcon, boss, num1,num2,num3,numG, liftRing, activeLiftRing;//, nano, zero;

bool hacking = false, doorLocked = true, hasKey = false, playMusic = true, hackLogUnlocked = false;
bool lvlLock[5];

MapEnum tile;
auto getTile = seqIntroEnum;
auto lastTile = seqIntroEnum;
auto lastMap = seqIntro;

char* message = "";
bool displayMessage = false;

int logAnimation = -36, logTimer = 0;

void debugFinalBoss(){
    getTile = finalEnum;
    BossFight::init(10);
    LevelManager::setLvl(4);
    camX = 10;
    camY = 10;
    PlayerManager::setDir(3);//face south
    LevelManager::setMap();
    state = FINAL_BOSS;
}

void unlockedLogAnimation(int y){
    HackLogManager::animateLog(y);
}

void gotoLift(){
    //TODO: lift music
    displayMessage = false;
    UI::clear();
    hasKey = false;
    prevState = state;
    PlayerManager::setDir(3);//face south
    LevelManager::setMap(lift, liftEnum);
    if(getTile == lobbyEnum) lvlLock[1] = false;
    if(getTile == lvl1Enum) lvlLock[2] = false;
    if(getTile == lvl2Enum) lvlLock[3] = false;
    if(getTile == lvl3Enum) lvlLock[4] = false;
    state = LIFT;
}

void gotoLevel(int id){
    if(lvlLock[id+1]){
        doorLocked = true;
        dor.play(door, Door::locked);
    }
    LevelManager::setLvl(id);
    LevelManager::setMap();
}

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
            state = ROBOPROGRAM;
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
            state = BRUTE_FORCE;
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
                state = SEQUENCE;
            }else{
                Sound::playSFX(Denied, sizeof(Denied));
                message = "I need a keycard to access\nthis terminal.";
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
        Sound::playSFX(Denied, sizeof(Denied));
        message = "Arg!\nThe sentry returned me to the exit.";
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
    
    if(hackLogUnlocked) openHackLog();
    
    if(hasKey) keyIcon.draw(16, 128);
    
    
    tileX = (camX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
    tileY = (camY + PlayerManager::getY() - 8 + PlayerManager::getH()) / PROJ_TILE_H;
    tile = getTile(tileX, tileY);
    
    if(tile == UnlockLogs){
        if(!hackLogUnlocked){
            HackLogManager::unlockLog(0);
            hackLogUnlocked = true;
            logAnimation = 16;
            logTimer = 50;
            message = "<BLIP!> What's this? My arm... (Press C to access logs)";
            displayMessage = true;
        }
    }
    
    if(tile == Log && Buttons::aBtn()){
        switch(state){
            case SEQ_INTRO:
                HackLogManager::unlockLog(2);
                logAnimation = 16;
                logTimer = 50;
                message = "This is the piece of Green Scrap that exploded in my face...";
                displayMessage = true;
                break;
            case BRUTE_INTRO:
                HackLogManager::unlockLog(9);
                logAnimation = 16;
                logTimer = 50;
                message = "This chip shouldn't be in here... What's going on?";
                displayMessage = true;
            break;
            case EXPLORE:
                if(getTile == lobbyEnum){
                    HackLogManager::unlockLog(7);
                    logAnimation = 16;
                    logTimer = 50;
                    message = "This must be the right place. The tech looks familiar...";
                    displayMessage = true;
                }
                if(getTile == lvl1Enum){
                    HackLogManager::unlockLog(11);
                    logAnimation = 16;
                    logTimer = 50;
                    message = "Where is everybody? Lights are on but nobody is home.";
                    displayMessage = true;
                }
                if(getTile == lvl2Enum){
                    HackLogManager::unlockLog(12);
                    logAnimation = 16;
                    logTimer = 50;
                    message = "Is it... a bad thing that I am starting to like this?";
                    displayMessage = true;
                }
                if(getTile == lvl3Enum){
                    
                }
            break;
        }
    }
    
    if(tile == Log2 && Buttons::aBtn()){
        switch(state){
            case EXPLORE:
                if(getTile == lobbyEnum){
                    HackLogManager::unlockLog(8);
                    logAnimation = 16;
                    logTimer = 50;
                    message = "This red glowing chip looks... evil... What happened here...";
                    displayMessage = true;
                }
                if(getTile == lvl1Enum){
                    HackLogManager::unlockLog(13);
                    logAnimation = 16;
                    logTimer = 50;
                    message = "These systems are encrypted, but I can still read them somehow.";
                    displayMessage = true;
                }
                if(getTile == lvl2Enum){
                    
                }
                if(getTile == lvl3Enum){
                    
                }
                
            break;
        }
    }
    
    if(tile == Ubuntu && Buttons::aBtn()){
        message = "Snap! These computers have been updated to use a GUI Desktop. That is Fancy.";
        displayMessage = true;
        if(!HackLogManager::checkUnlocked(10)){
            HackLogManager::unlockLog(10);
            logAnimation = 16;
            logTimer = 50;  
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
    boss.play(finalBoss, FinalBoss::idle);
    
    liftRing.play(liftSelector, LiftSelector::idle);
    activeLiftRing.play(liftSelector, LiftSelector::hover);
    
    num1.play(floorNumbers, FloorNumbers::one);
    num2.play(floorNumbers, FloorNumbers::two);
    num3.play(floorNumbers, FloorNumbers::three);
    numG.play(floorNumbers, FloorNumbers::ground);
    
    for(int i = 0; i < 5; ++i){
        lvlLock[i] = true;
    }
    
    Display::loadRGBPalette(miloslav);
    LevelManager::init();
    LevelManager::setMap(seqIntro, seqIntroEnum);
    PlayerManager::init();
    HackLogManager::init();
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
    
    if(logAnimation > -20){
        if(logTimer > 0) logTimer--;
        
        if(logTimer == 0) logAnimation--;
        
        unlockedLogAnimation(logAnimation);
    }
    
    if(playMusic)Sound::playMusicStream();
    else Sound::pauseMusicStream();
    
    switch(state){
    case INTRO:
        if(Buttons::downBtn()){
            debugFinalBoss();
            return;
        }
        if(Buttons::upBtn()){
            for(int i = 0; i < 5; ++i){
                lvlLock[i] = false;
            }
            gotoLift();
            return;
        }
        
        UI::drawGauge(8, 28, 17, introProg, 100);

        if( Buttons::cBtn() ){
            introProg++;
            if(introProg == 100){
                UI::clear();
                state = SEQ_INTRO;
                Sound::playMusicStream("music/npz-a.raw", 0);
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
            if(prevState == BRUTE_INTRO){
                HackLogManager::unlockLog(5);
                logAnimation = 16;
                logTimer = 50;
            }
            if(getTile == lvl1Enum){
                HackLogManager::unlockLog(12);
                logAnimation = 16;
                logTimer = 50;
            }
            if(getTile == lvl2Enum){
                HackLogManager::unlockLog(12);
                logAnimation = 16;
                logTimer = 50;
            }
            if(getTile == lvl3Enum){
                HackLogManager::unlockLog(12);
                logAnimation = 16;
                logTimer = 50;
            }
            Sound::playSFX(Unlock, sizeof(Unlock));
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
                logAnimation = 16;
                logTimer = 50;
            }
            if(prevState == ROBO_INTRO){
                HackLogManager::unlockLog(4);
                logAnimation = 16;
                logTimer = 50;
            }
            if(prevState == BRUTE_INTRO){
                HackLogManager::unlockLog(6);
                logAnimation = 16;
                logTimer = 50;
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
        
        for(int i = 0; i < 6; ++i){
            hack.draw(12, 40 + i*16);
        }
        for(int i = 0; i < 6; ++i){
            hack.draw(LCDWIDTH-26, 40 + i*16);
        }

        if(RoboHack::complete()){
            if(RoboHack::keyReturned()){
                Sound::playSFX(Unlock, sizeof(Unlock));
                hasKey = true;
                message = "I got the keycard!";
                displayMessage = true;
                HackLogManager::unlockLog(3);
                logAnimation = 16;
                logTimer = 50;
            }
            
            if(prevState != EXPLORE) LevelManager::setMap(lastMap, lastTile);
            else LevelManager::setMap();
            
            getTile = lastTile;
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
                lastMap = roboIntro;
                LevelManager::setMap(roboIntro, roboIntroEnum);
                getTile = roboIntroEnum;
                state = ROBO_INTRO;
            }
            if(introLevel == 2){
                lastMap = bruteIntro;
                LevelManager::setMap(bruteIntro, bruteIntroEnum);
                LevelManager::setDroneState(true);
                getTile = bruteIntroEnum;
                state = BRUTE_INTRO;
                
            }
            if(introLevel == 3){
                lastMap = lobby;
                LevelManager::setMap(lobby, lobbyEnum);
                LevelManager::setDroneState(true);
                getTile = lobbyEnum;
                state = EXPLORE;
                camX = 10;
                camY = 10;
                lvlLock[0] = false;
                Sound::playMusicStream("music/npz-e.raw", 0);
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
            gotoLift();
        }
    break;
    case LIFT:
        LevelManager::render(0, 0);
        
        if(Buttons::aBtn()){
            if(!lvlLock[lvlSelect]){
                gotoLevel(lvlSelect);
                camX = 10;
                camY = 10;
                PlayerManager::setDir(3);//face south
                    switch(lvlSelect){
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
                            getTile = lvl3Enum;
                            break;
                        case 4: //final room
                            getTile = finalEnum;
                            BossFight::init(10);
                            break;
                    }
                if(LevelManager::getLvl() < 4){
                    LevelManager::setDroneState(true);
                    state = EXPLORE;
                }else {
                    state = FINAL_BOSS;
                }
            }else {
                Sound::playSFX(Denied, sizeof(Denied));
            }
        }
        
        if(Buttons::upBtn()){
            if(lvlSelect == 0) lvlSelect = 2;
            if(lvlSelect == 1) lvlSelect = 3;
            if(lvlSelect == 3) lvlSelect = 4;
        }

        if(Buttons::downBtn()){
            if(lvlSelect == 2) lvlSelect = 0;
            if(lvlSelect == 3) lvlSelect = 1;
            if(lvlSelect == 4) lvlSelect = 3;
        }
        
        if(Buttons::leftBtn()){
            if(lvlSelect == 3) lvlSelect = 2;
            if(lvlSelect == 1) lvlSelect = 0; 
        }
        if(Buttons::rightBtn()){
            if(lvlSelect == 0) lvlSelect = 1;
            if(lvlSelect == 2) lvlSelect = 3;
        }
        
        if(lvlSelect == 4) activeLiftRing.draw(108, 24);
        else liftRing.draw(108, 24);
        
        if(lvlSelect == 3) activeLiftRing.draw(lrRight, lrTop);
        else liftRing.draw(lrRight, lrTop);
        
        if(lvlSelect == 1) activeLiftRing.draw(lrRight, lrBottom);
        else liftRing.draw(lrRight, lrBottom);
        
        if(lvlSelect == 2) activeLiftRing.draw(lrLeft, lrTop);
        else liftRing.draw(lrLeft, lrTop);
        
        if(lvlSelect == 0) activeLiftRing.draw(lrLeft, lrBottom);
        else liftRing.draw(lrLeft, lrBottom);
        
        
        if(lvlSelect == 0 && !lvlLock[0]) numG.play(floorNumbers, FloorNumbers::groundActive);
        else numG.play(floorNumbers, FloorNumbers::ground);
        
        if(lvlSelect == 1 && !lvlLock[1]) num1.play(floorNumbers, FloorNumbers::oneActive);
        else num1.play(floorNumbers, FloorNumbers::one);
        
        if(lvlSelect == 2 && !lvlLock[2]) num2.play(floorNumbers, FloorNumbers::twoActive);
        else num2.play(floorNumbers, FloorNumbers::two);
        
        if(lvlSelect == 3 && !lvlLock[3]) num3.play(floorNumbers, FloorNumbers::threeActive);
        else num3.play(floorNumbers, FloorNumbers::three);
        
        num3.draw(lrRight+8, lrTop+8);
        num1.draw(lrRight+8, lrBottom+8);
        num2.draw(lrLeft+8, lrTop+8);
        numG.draw(lrLeft+8, lrBottom+8);
        
    break;
    case HACK_LOG:
        HackLogManager::update();
        HackLogManager::render();
        
        if(HackLogManager::shouldEnd()){
            UI::clear();
            UI::showTileMapSpritesUI();
            playMusic = HackLogManager::getPlayMusic();
            state = prevState;
        }
    break;
    case FINAL_BOSS:
        //TODO: final battle theme
        LevelManager::render(-camX, -camY);
        dor.draw(-camX+96, -camY+80);
        dor.draw(-camX+112, -camY+80);
        
        oldX = camX;
        oldY = camY;
        
        PlayerManager::render();
                
        if(BossFight::loop()){
            if(!BossFight::complete()){
                boss.draw(-camX + 160, - camY + 48);
                BossFight::update();
                BossFight::render();
            }else {
                doorLocked = false;
                dor.play(door, Door::unlocked);
                hasKey = true;
            
                if(BossFight::getVictory()) state = END_GAME;
                
                BossFight::endLoop();
            }
        }
        
        if(!BossFight::running()){
            PlayerManager::update(camX, camY);
            
            tileX = (camX + PlayerManager::getX() + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (camY + PlayerManager::getY() - 8 + PlayerManager::getH()) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            checkCollide();
            checkDoor();
            if( tile == Fight ){
                hack.draw(PlayerManager::getX(), PlayerManager::getY()-16, false, false);
                if(Buttons::cBtn()){
                    BossFight::resume();
                }
            }
            if( tile == Lift ){
                state = END_GAME;
            }
                    
            if(hasKey){
                keyIcon.draw(16, 128);
            }
        }
        // openHackLog();
    break;
    case END_GAME:
        UI::clear();
        UI::drawBox(messageX, messageY, 32, messageY + 4);
        UI::setCursorBoundingBox(messageX + 1, messageY + 1, 32 - 1, messageY + 4 - 1);
        UI::setCursor(messageX + 1, messageY + 1);
        UI::setCursorDelta(UIVariants::standard);
        UI::printText("Thanks for playing! Coming soon: More logs. More story.");
        if(Buttons::cBtn()){
            gotoLift();
        }
    break;
    }
}
