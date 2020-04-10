#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

enum Inputs{
    A, B, C, UP, DOWN, LEFT, RIGHT
};

enum State{
    INTRO, HACKING, EXPLORE
};

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
    
    int cameraX = 12, cameraY = 12, speed = 1, recolor = 0;
    
    
    
    Sprite player;
    Sprite hack;
    Sprite icons;
    Sprite pBar;
    
    Inputs order[7] = {UP, A, RIGHT, B, C, DOWN, LEFT};
    bool orderStatus[7] = {false, false, false, false, false, false, false};
    
    
    pBar.play(progBar, ProgBar::start);
    icons.play(hackIcons, HackIcons::bUp);
    hack.play(hackme, Hackme::show);
    player.play(hero, Hero::walkSouth);
    auto playerWidth = player.getFrameWidth();
    auto playerHeight = player.getFrameHeight();
    auto playerX = LCDWIDTH/2 - playerWidth/2;
    auto playerY = LCDHEIGHT/2 - playerHeight/2;
    
    State state = State::INTRO;
    bool hacking = false, intro = true, bpress = false, apress = false, cpress = false;
    
    int prog = 0, percent = 0, select = 0;
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        switch(state){
        case INTRO:
        
            switch(order[select]){
                case A:
                    if( Buttons::pressed(BTN_A)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case B:
                    if( Buttons::pressed(BTN_B)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case C:
                    if( Buttons::pressed(BTN_C)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case UP:
                    if( Buttons::pressed(BTN_UP)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case DOWN:
                    if( Buttons::pressed(BTN_DOWN)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case LEFT:
                    if( Buttons::pressed(BTN_LEFT)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case RIGHT:
                    if( Buttons::pressed(BTN_RIGHT)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
            }
            
            for(int x = 0; x < 7; ++x){
                switch(order[x]){
                    case A:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::aDown);
                        }else{
                            icons.play(hackIcons, HackIcons::aUp);
                        }
                        break;
                    case B:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::bDown);
                        }else {
                            icons.play(hackIcons, HackIcons::bUp);
                        }
                        break;
                    case C:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::cDown);
                        }else{
                            icons.play(hackIcons, HackIcons::cUp);
                        }
                        break;
                    case UP:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dUpDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dUpUp);
                        }
                        break;
                    case DOWN:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dDownDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dDownUp);
                        }
                        break;
                    case LEFT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dLeftDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dLeftUp);
                        }
                        break;
                    case RIGHT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dRightDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dRightUp);
                        }
                        break;
                }
                icons.draw(16 + x * 16, 32);
            }

            if(select == 0){
                pBar.play(progBar, ProgBar::first);
            }
            if(select == 2){
                pBar.play(progBar, ProgBar::second);
            }
            if(select == 4){
                pBar.play(progBar, ProgBar::third);
            }
            if(select == 6){
                pBar.play(progBar, ProgBar::final);
            }

            if(select == 7){
                select = 0;
                state = State::EXPLORE;
            }
            Display::setColor(7);
            Display::print(prog);
            pBar.draw(10, 10);
            
            break;
        case HACKING:
        
            // Debug state switch
            // if( Buttons::bBtn() ){
            //     state = State::EXPLORE;
            // }
        
            hack.draw(playerX, playerY-16, false, false);
             // draw map
            tilemap.draw(-cameraX, -cameraY);
            
            // draw hero player
            player.draw(playerX, playerY, false, false, recolor);
            
            
            switch(order[select]){
                case A:
                    if( Buttons::pressed(BTN_A)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case B:
                    if( Buttons::pressed(BTN_B)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case C:
                    if( Buttons::pressed(BTN_C)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case UP:
                    if( Buttons::pressed(BTN_UP)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case DOWN:
                    if( Buttons::pressed(BTN_DOWN)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case LEFT:
                    if( Buttons::pressed(BTN_LEFT)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
                case RIGHT:
                    if( Buttons::pressed(BTN_RIGHT)){
                        orderStatus[select] = true;
                        select++;
                    }
                    break;
            }
            
            for(int x = 0; x < 7; ++x){
                switch(order[x]){
                    case A:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::aDown);
                        }else{
                            icons.play(hackIcons, HackIcons::aUp);
                        }
                        break;
                    case B:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::bDown);
                        }else {
                            icons.play(hackIcons, HackIcons::bUp);
                        }
                        break;
                    case C:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::cDown);
                        }else{
                            icons.play(hackIcons, HackIcons::cUp);
                        }
                        break;
                    case UP:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dUpDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dUpUp);
                        }
                        break;
                    case DOWN:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dDownDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dDownUp);
                        }
                        break;
                    case LEFT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dLeftDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dLeftUp);
                        }
                        break;
                    case RIGHT:
                        if(orderStatus[x]){
                            icons.play(hackIcons, HackIcons::dRightDown);
                        }else{
                            icons.play(hackIcons, HackIcons::dRightUp);
                        }
                        break;
                }
                icons.draw(16 + x * 16, 32);
            }

            if(select == 0){
                pBar.play(progBar, ProgBar::first);
            }
            if(select == 2){
                pBar.play(progBar, ProgBar::second);
            }
            if(select == 4){
                pBar.play(progBar, ProgBar::third);
            }
            if(select == 6){
                pBar.play(progBar, ProgBar::final);
            }

            if(select == 7){
                select = 0;
                state = State::EXPLORE;
            }
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
                    for(int i = 0; i < 7; ++i){
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
