#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

enum State{
    INTRO, HACKING, EXPLORE
};

static const uint8_t B_UP = 1 << UPBIT;
static const uint8_t B_DOWN = 1 << DOWNBIT;
static const uint8_t B_LEFT = 1 << LEFTBIT;
static const uint8_t B_RIGHT = 1 << RIGHTBIT;
static const uint8_t B_A = 1 << ABIT;
static const uint8_t B_B = 1 << BBIT;
static const uint8_t B_C = 1 << CBIT;

int main(){
    using Pokitto::Core;
    using Pokitto::Display;
    using Pokitto::Buttons;
    
    Core::begin();
    Display::loadRGBPalette(miloslav);
    
    Tilemap tilemap;
    //0 = 14, 1 = 11, suburb+2 = mapPixelData
    tilemap.set(suburb[0], suburb[1], suburb+2);
    for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
        tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
    }
    
    // set background to blank black tile
    tilemap.setColorTile(0,0);
    
    int cameraX = 12, cameraY = 12, speed = 1, recolor = 0, seqSize = 10;
    
    
    
    Sprite player;
    Sprite hack;
    Sprite icons;
    Sprite pBar;
    Sprite pFill;
    Sprite sent;
    Sprite tLevel;
    
    
    pBar.play(progBar, ProgBar::start);
    pFill.play(progFill, ProgFill::play);
    icons.play(hackIcons, HackIcons::bUp);
    hack.play(hackme, Hackme::show);
    player.play(hero, Hero::walkSouth);
    sent.play(sentinal, Sentinal::scanning);
    tLevel.play(threatLevel, ThreatLevel::zero);
    
    auto playerWidth = player.getFrameWidth();
    auto playerHeight = player.getFrameHeight();
    auto playerX = LCDWIDTH/2 - playerWidth/2;
    auto playerY = LCDHEIGHT/2 - playerHeight/2;
    
    State state = State::INTRO;
    bool hacking = false, intro = true, bpress = false, apress = false, cpress = false;
    
    int prog = 0, percent = 0, select = 0, threat = 0;
    
    
    // init button state mechanism
    uint8_t buttonsPreviousState = 0, buttonsJustPressed = 0;
    
    // init hacking sequence variables.
    uint8_t order[seqSize];
    srand((unsigned int) time (NULL));
    
    for(int i = 0; i < seqSize; ++i){
        int r = rand()%7;
        switch(r){
            case 0:
                order[i] = B_A;
                break;
            case 1:
                order[i] = B_B;
                break;
            case 2:
                order[i] = B_C;
                break;
            case 3:
                order[i] = B_UP;
                break;
            case 4:
                order[i] = B_RIGHT;
                break;
            case 5:
                order[i] = B_DOWN;
                break;
            case 6:
                order[i] = B_LEFT;
                break;
        }
    }
    
    bool orderStatus[seqSize];
    for(int i = 0; i < seqSize; ++i){
        orderStatus[i] = false;   
    }
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        switch(state){
        case INTRO:

            if (buttonsJustPressed & order[select] ){
                orderStatus[select] = true;
                select++;
            } else if (buttonsJustPressed != 0) {
                threat++;
            }
            
            for(int x = 0; x < seqSize; ++x){
                switch(order[x]){
                    case B_A:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::aDown);
                        }else{
                            icons.play(hackIcons, HackIcons::aUp);
                        }
                        break;
                    case B_B:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::bDown);
                        }else {
                            icons.play(hackIcons, HackIcons::bUp);
                        }
                        break;
                    case B_C:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::cDown);
                        }else{
                            icons.play(hackIcons, HackIcons::cUp);
                        }
                        break;
                    case B_UP:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dUpDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dUpUp);
                        }
                        break;
                    case B_DOWN:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dDownDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dDownUp);
                        }
                        break;
                    case B_LEFT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dLeftDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dLeftUp);
                        }
                        break;
                    case B_RIGHT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dRightDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dRightUp);
                        }
                        break;
                }
                icons.draw(16 + x * 16, 32);
            }
            
            switch(threat){
                case 1:
                    tLevel.play(threatLevel, ThreatLevel::one);
                    break;
                case 2:
                    tLevel.play(threatLevel, ThreatLevel::two);
                    break;
                case 3:
                    tLevel.play(threatLevel, ThreatLevel::three);
                    break;
                case 4:
                    tLevel.play(threatLevel, ThreatLevel::four);
                    break;
                case 5:
                    tLevel.play(threatLevel, ThreatLevel::five);
                    sent.play(sentinal, Sentinal::warning);
                    threat++;
                    break;
            }
            
            if(threat > 8){
                //lose
            }


            if(select == seqSize){
                select = 0;
                state = State::EXPLORE;
            }
            
            for(int i = 0; i < select; ++i){
                pFill.draw(15 + i * 15, 14);
            }
            
            sent.draw(200, 50);
            tLevel.draw(200, 70);
            
            Display::setColor(7);
            Display::print(prog);
            pBar.draw(10, 10);
            
            break;
        case HACKING:
        
            hack.draw(playerX, playerY-16, false, false);
             // draw map
            tilemap.draw(-cameraX, -cameraY);
            
            // draw hero player
            player.draw(playerX, playerY, false, false, recolor);
            
            
            if (buttonsJustPressed & order[select] ){
                orderStatus[select] = true;
                select++;
            } else if (buttonsJustPressed != 0) {
                threat++;
            }
            
            for(int x = 0; x < seqSize; ++x){
                switch(order[x]){
                    case B_A:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::aDown);
                        }else{
                            icons.play(hackIcons, HackIcons::aUp);
                        }
                        break;
                    case B_B:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::bDown);
                        }else {
                            icons.play(hackIcons, HackIcons::bUp);
                        }
                        break;
                    case B_C:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::cDown);
                        }else{
                            icons.play(hackIcons, HackIcons::cUp);
                        }
                        break;
                    case B_UP:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dUpDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dUpUp);
                        }
                        break;
                    case B_DOWN:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dDownDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dDownUp);
                        }
                        break;
                    case B_LEFT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dLeftDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dLeftUp);
                        }
                        break;
                    case B_RIGHT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dRightDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dRightUp);
                        }
                        break;
                }
                icons.draw(16 + x * 16, 32);
            }
            
            switch(threat){
                case 1:
                    tLevel.play(threatLevel, ThreatLevel::one);
                    break;
                case 2:
                    tLevel.play(threatLevel, ThreatLevel::two);
                    break;
                case 3:
                    tLevel.play(threatLevel, ThreatLevel::three);
                    break;
                case 4:
                    tLevel.play(threatLevel, ThreatLevel::four);
                    break;
                case 5:
                    tLevel.play(threatLevel, ThreatLevel::five);
                    sent.play(sentinal, Sentinal::warning);
                    threat++;
                    break;
            }
            
            if(threat > 8){
                //lose
            }

            if(select == seqSize){
                select = 0;
                state = State::EXPLORE;
            }
            
            for(int i = 0; i < select; ++i){
                pFill.draw(15 + i * 15, 14);
            }
            
            sent.draw(200, 50);
            tLevel.draw(200, 70);
            
            Display::setColor(7);
            Display::print(prog);
            pBar.draw(10, 10);
            
            
            Display::print(100, 0, "Have to hack!");
            
            break;
        case EXPLORE:
        
             
            int oldX = cameraX;
            int oldY = cameraY;
            
            if(!Buttons::rightBtn() && !Buttons::leftBtn() && !Buttons::upBtn() && !Buttons::downBtn()){
                if(player.animation == Hero::walkEast){
                    player.play(hero, Hero::idleEast);
                }
                if(player.animation == Hero::walkWest){
                    player.play(hero, Hero::idleWest);
                }
                if(player.animation == Hero::walkNorth){
                    player.play(hero, Hero::idleNorth);
                }
                if(player.animation == Hero::walkSouth){
                    player.play(hero, Hero::idleSouth);
                }
            }
    
    
            if(Buttons::leftBtn() || Buttons::rightBtn() ){
                if(Buttons::rightBtn()){
                    cameraX += speed;
                    if(player.animation != Hero::walkEast){
                        player.play(hero, Hero::walkEast);
                    }
                }
                if(Buttons::leftBtn()){
                    cameraX -= speed;
                    if(player.animation != Hero::walkWest){
                        player.play(hero, Hero::walkWest);
                    }
                }
            }else{
                
                if(Buttons::upBtn()){
                    cameraY -= speed;
                    if(player.animation != Hero::walkNorth){
                        player.play(hero, Hero::walkNorth);
                    }
                }
                if(Buttons::downBtn()){
                    cameraY += speed;
                    if(player.animation != Hero::walkSouth){
                        player.play(hero, Hero::walkSouth);
                    }
                }
            }
            
            
            // get current tile
            int tileX = (cameraX + playerX + PROJ_TILE_W/2) / PROJ_TILE_W;
            int tileY = (cameraY + playerY + playerHeight) / PROJ_TILE_H;
            auto tile = suburbEnum(tileX, tileY);
    
            if( tile&Hack ){
                // Eventually play a Hack tone here, draw hack when actually hacking
                // Play hackable notification 
                
                
                if( Buttons::aBtn() ){
                    prog = 0;
                    pBar.play(progBar, ProgBar::start);
                    select = 0;
                    threat = 0;
                    srand((unsigned int) time (NULL));
                    
                    for(int i = 0; i < seqSize; ++i){
                        int r = rand()%7;
                        switch(r){
                            case 0:
                                order[i] = B_A;
                                break;
                            case 1:
                                order[i] = B_B;
                                break;
                            case 2:
                                order[i] = B_C;
                                break;
                            case 3:
                                order[i] = B_UP;
                                break;
                            case 4:
                                order[i] = B_RIGHT;
                                break;
                            case 5:
                                order[i] = B_DOWN;
                                break;
                            case 6:
                                order[i] = B_LEFT;
                                break;
                        }
                    }
                    
                    for(int i = 0; i < seqSize; ++i){
                        orderStatus[i] = false;   
                    }
                    state = State::HACKING;
                }
            }
            
            if( tile&Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
    
    
            // draw map
            tilemap.draw(-cameraX, -cameraY);
            
            // draw hero player
            player.draw(playerX, playerY, false, false, recolor);
            
            
            Display::setColor(7);
            Display::print("How do I get out of here?");
            break;
        }

    }
    
    return 0;
}
