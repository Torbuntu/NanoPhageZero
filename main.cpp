#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

enum State{
    INTRO,
    HACKING,
    EXPLORE
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
    
    int prog = 0, percent = 0;
    
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        switch(state){
        case INTRO:
            if( prog <= 3 ){
                
                if( Buttons::pressed(BTN_B) ){
                    pBar.play(progBar, ProgBar::first);
                    prog++;
                    icons.play(hackIcons, HackIcons::bDown);
                }
                if( !Buttons::bBtn() ){
                    icons.play(hackIcons, HackIcons::bUp);
                }
                
                if(Buttons::cBtn() || Buttons::pressed(BTN_A)){
                    prog--;
                }
            }else if( prog <= 6 ){
                
                if( Buttons::pressed(BTN_A) ){
                    pBar.play(progBar, ProgBar::second);
                    prog++;
                    icons.play(hackIcons, HackIcons::aDown);
                }
                if( !Buttons::aBtn() ){
                    icons.play(hackIcons, HackIcons::aUp);
                }
                if(Buttons::pressed(BTN_B) || Buttons::cBtn()){
                    prog--;
                }
            }else {
                
                if( Buttons::pressed(BTN_C) ){
                    pBar.play(progBar, ProgBar::third);
                    prog++;
                    icons.play(hackIcons, HackIcons::cDown);
                }
                if( !Buttons::cBtn() ){
                    icons.play(hackIcons, HackIcons::cUp);
                }
                if(Buttons::pressed(BTN_B) || Buttons::pressed(BTN_A)){
                    prog--;
                }
            }
            
            if(prog < 0){
                prog = 0;
            }
            
            if(prog == 10){
                pBar.play(progBar, ProgBar::final);
            }
            
            if( prog > 10 ){
                prog = 0;
                state = State::EXPLORE;
            }
            
            Display::setColor(7);
            Display::print(prog);
            icons.draw(12, 32);
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
            
            
            if( prog <= 18 ){
                if(prog == 1){
                     pBar.play(progBar, ProgBar::first);
                }
                if( Buttons::pressed(BTN_B) ){
                    prog++;
                    icons.play(hackIcons, HackIcons::bDown);
                }
                if( !Buttons::bBtn() ){
                    icons.play(hackIcons, HackIcons::bUp);
                }
                
                if(Buttons::cBtn() || Buttons::pressed(BTN_A)){
                    prog--;
                }
            }else if( prog <= 38 ){
                if(prog == 19){
                    pBar.play(progBar, ProgBar::second);
                }
                if( Buttons::pressed(BTN_A) ){
                    prog++;
                    icons.play(hackIcons, HackIcons::aDown);
                }
                if( !Buttons::aBtn() ){
                    icons.play(hackIcons, HackIcons::aUp);
                }
                if(Buttons::pressed(BTN_B) || Buttons::cBtn()){
                    prog--;
                }
            }else {
                if(prog == 39){
                    pBar.play(progBar, ProgBar::third);
                }
                if( Buttons::pressed(BTN_C) ){
                    prog++;
                    icons.play(hackIcons, HackIcons::cDown);
                }
                if( !Buttons::cBtn() ){
                    icons.play(hackIcons, HackIcons::cUp);
                }
                if(Buttons::pressed(BTN_B) || Buttons::pressed(BTN_A)){
                    prog--;
                }
            }
            
            if(prog < 0){
                prog = 0;
            }
            
            if(prog == 50){
                pBar.play(progBar, ProgBar::final);
            }
            
            if( prog > 50 ){
                prog = 0;
                state = State::EXPLORE;
            }
            
            percent = prog / 50.0 * 100;
            
            Display::setColor(7);
            Display::print(percent);
            Display::print("%");
            icons.draw(12, 32);
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
