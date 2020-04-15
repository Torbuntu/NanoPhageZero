#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

#include "Sequence.hpp"

enum State{
    INTRO, BRUTE_FORCE, SEQUENCE, EXPLORE, TUT_BRUT, TUT_SEQ
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
    using Sequence::SeqHack;
    
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
    
    int cameraX = 12, cameraY = 12, speed = 1, recolor = 0, bruteCount = 0, bruteSelect = B_A, bruteProgress = 0, enemyProgress = 0, enemyTimer = 0, enemySpeed = 0;
    
    Sprite player, hack, eFill, virus;
    
    eFill.play(enemyProgFill, EnemyProgFill::play);
    virus.play(enemyVirus, EnemyVirus::play);
    
    player.play(hero, Hero::walkSouth);
    hack.play(hackme, Hackme::show);
    
    auto playerWidth = player.getFrameWidth();
    auto playerHeight = player.getFrameHeight();
    auto playerX = LCDWIDTH/2 - playerWidth/2;
    auto playerY = LCDHEIGHT/2 - playerHeight/2;
    
    State state = State::INTRO;
    bool hacking = false, doorLocked = true;
    
    while( Core::isRunning() ){
        if( !Core::update() ) {
            continue;
        }
        
        // Poll the button: moved to Sequence.cpp
        // Buttons::pollButtons();
        // buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        // buttonsPreviousState = Buttons::buttons_state;
        
        switch(state){
        case TUT_BRUT:
            
            // if(buttonsJustPressed & bruteSelect){
            //     select++;
            //     bruteProgress++;
            // }
            // if( select == bruteCount ){
            //     int r = rand()%7;
            //     switch(r){
            //         case 0:
            //             bruteSelect = B_A;
            //             icons.play(hackIcons, HackIcons::aUp);
            //             break;
            //         case 1:
            //             bruteSelect = B_B;
            //             icons.play(hackIcons, HackIcons::bUp);
            //             break;
            //         case 2:
            //             bruteSelect = B_C;
            //             icons.play(hackIcons, HackIcons::cUp);
            //             break;
            //         case 3:
            //             bruteSelect = B_UP;
            //             icons.play(hackIcons, HackIcons::dUpUp);
            //             break;
            //         case 4:
            //             bruteSelect = B_RIGHT;
            //             icons.play(hackIcons, HackIcons::dRightUp);
            //             break;
            //         case 5:
            //             bruteSelect = B_DOWN;
            //             icons.play(hackIcons, HackIcons::dDownUp);
            //             break;
            //         case 6:
            //             bruteSelect = B_LEFT;
            //             icons.play(hackIcons, HackIcons::dLeftUp);
            //             break;
            //     }
            //     select = 0;
            // }
            
            // // icon to press
            // icons.draw(8, 32);
            
            // // player bar
            // pBar.draw(10,10);
            // for(int i = 0; i < bruteProgress; ++i){
            //     pFill.draw(14 + i * 6 , 14);
            // }
            
            
            // enemyTimer++;
            // if(enemyTimer == enemySpeed){
            //     enemyTimer = 0;
            //     if(enemyProgress < 30){
            //         enemyProgress++;
            //     }
                
            // }
            // // enemy bar
            // pBar.draw(10,150);
            // for(int i = 0; i < enemyProgress; ++i){
            //     eFill.draw(190 - i * 6 , 154);
            // }
            
            // virus.draw(190, 130);
            
            
            // if(bruteProgress >= 30){
            //     state = State::INTRO;
            // }
            
            break;
        case TUT_SEQ:
            SeqHack::update();
            SeqHack::render();
            
            if(SeqHack::complete()){
                state = State::INTRO;
            }
            
            if(SeqHack::fail()){
                state = State::INTRO;
            }
            break;
        case INTRO:
            if( Buttons::leftBtn() ){
                SeqHack::init();
                SeqHack::shuffle(6);
                state = State::TUT_SEQ;
            }
            
            if( Buttons::rightBtn() ){
                // bruteCount = 5;
                // select = 0;
                // bruteSelect = B_A;
                // bruteProgress = 0;
                // enemyProgress = 1;
                // enemySpeed = 15;
                // enemyTimer = 0;
                // icons.play(hackIcons, HackIcons::aUp);
                // srand((unsigned int) time (NULL));
                // state = State::TUT_BRUT;   
            }
            
            if( Buttons::cBtn() ){
                state = State::EXPLORE;
            }
            
            Display::setColor(7);
            Display::println("Sequence Tutorial: <-");
            Display::println("Brute Force Tutorial: ->");
            Display::print("C to start.");
            
            break;
        case SEQUENCE:
        
            // draw the hacking icon above player
            hack.draw(playerX, playerY-16, false, false);
             // draw map
            tilemap.draw(-cameraX, -cameraY);
            
            // draw hero player
            player.draw(playerX, playerY, false, false, recolor);
            
            SeqHack::update();
            
            if(SeqHack::fail()){
                //lose
                state = State::EXPLORE;
                doorLocked = true;
            }

            if(SeqHack::complete()){
                doorLocked = false;
                tilemap.setColorTile(3, 0);
                state = State::EXPLORE;
            }
            
            SeqHack::render();
            
            Display::setColor(7);
            Display::print(0, 0, "Hack the planet!");
            
            break;
        case EXPLORE:
             
            int oldX = cameraX;
            int oldY = cameraY;
            
            // We don't poll for movement because it will be held down.
            if(!Buttons::rightBtn() && !Buttons::leftBtn() && !Buttons::upBtn() && !Buttons::downBtn()){
                switch(player.animation){
                    case Hero::walkEast:
                        player.play(hero, Hero::idleEast);
                        break;
                    case Hero::walkWest:
                        player.play(hero, Hero::idleWest);
                        break;
                    case Hero::walkNorth:
                        player.play(hero, Hero::idleNorth);
                        break;
                    case Hero::walkSouth:
                        player.play(hero, Hero::idleSouth);
                        break;
                }
                
                if(doorLocked){
                    Display::print("How do I get out of here?");
                }else {
                    Display::print("The door is open!");
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
            int tileY = (cameraY + playerY - 8 + playerHeight) / PROJ_TILE_H;
            auto tile = suburbEnum(tileX, tileY);
    
            if( tile == Hack ){
                // Eventually play a Hack tone here, draw hack when actually hacking
                // Play hackable notification 
                if( Buttons::aBtn() ){
                    SeqHack::init();
                    SeqHack::shuffle(13);
                    state = State::SEQUENCE;
                }
            }
            
            if( tile == Collide ){
                cameraX = oldX;
                cameraY = oldY;
            }
            
            if( tile == Door && doorLocked ){
                if(doorLocked){
                    cameraX = oldX;
                    cameraY = oldY;
                    Display::print("The door is locked! Where is the control system?");
                }else {
                    // if !doorLocked enter next level
                }
            }
    
            // draw map
            tilemap.draw(-cameraX, -cameraY);
            
            // draw hero player
            player.draw(playerX, playerY, false, false, recolor);
            
            
            Display::setColor(7);
            
            break;
        }

    }
    
    return 0;
}
