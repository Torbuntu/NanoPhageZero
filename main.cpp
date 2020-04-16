#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

#include "Minigames/Sequence.hpp"
#include "Minigames/Bruteforce.hpp"

enum State{
    INTRO, BRUTE_FORCE, SEQUENCE, EXPLORE, TUT_BRUT, TUT_SEQ
};

int main(){
    using Pokitto::Core;
    using Pokitto::Display;
    using Pokitto::Buttons;
    using Sequence::SeqHack;
    using Bruteforce::BruteHack;
    
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
    
    Sprite player, hack;
    
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

        switch(state){
        case TUT_BRUT:
            
            BruteHack::update();
            
            BruteHack::render();
            
            if(BruteHack::complete()){
                state = State::INTRO;
            }
            
            if(BruteHack::fail()){
                state = State::INTRO;
            }
            
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
                BruteHack::init();
                state = State::TUT_BRUT;   
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
