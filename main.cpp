#include <Pokitto.h>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "maps.h"

#include "Minigames/Sequence.hpp"
#include "Minigames/Bruteforce.hpp"
#include "Minigames/RobotProgram.hpp"

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
    
    Core::begin();
    Display::loadRGBPalette(miloslav);
    
    Tilemap tilemap;
    //0 = 14, 1 = 11, suburb+2 = mapPixelData
    tilemap.set(seqIntro[0], seqIntro[1], seqIntro+2);
    for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
        tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
    }
    
    // set background to blank black tile
    //tilemap.setColorTile(100,0);
    tilemap.fillOutOfBounds = 255;
    
    int cameraX = 12, cameraY = 12, speed = 1, recolor = 0, tileX, tileY, oldX, oldY, doorX, doorY;
    
    Sprite player, hack, dor, botField;
    
    botField.play(minibotField, MinibotField::idle);
    player.play(hero, Hero::walkSouth);
    hack.play(hackme, Hackme::show);
    dor.play(door, Door::locked);
    
    auto playerWidth = player.getFrameWidth();
    auto playerHeight = player.getFrameHeight();
    auto playerX = LCDWIDTH/2 - playerWidth/2;
    auto playerY = LCDHEIGHT/2 - playerHeight/2;
    
    State state = State::EXPLORE;
    State prevState = State::EXPLORE;
    
    bool hacking = false, doorLocked = true;
    MapEnum tile;
    auto getTile = seqIntroEnum;
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
            tilemap.draw(-cameraX, -cameraY);
            dor.draw(doorX, doorY);
            player.draw(playerX, playerY, false, false, recolor);
            
            BruteHack::update();
            BruteHack::render();
            break;
        case SEQUENCE:
            tilemap.draw(-cameraX, -cameraY);
            dor.draw(doorX, doorY);
            player.draw(playerX, playerY, false, false, recolor);
            
            // draw the hacking icon above player
            hack.draw(playerX, playerY-16, false, false);
            
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
            tilemap.draw(-RoboHack::getRobotX(), -RoboHack::getRobotY());

            if(RoboHack::complete()){
                doorLocked = false;
                dor.play(door, Door::unlocked);
                tilemap.set(roboIntro[0], roboIntro[1], roboIntro+2);
                for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
                    tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
                }
                getTile = roboIntroEnum;
                state = prevState;
            }
        
            break;
        case EXPLORE:
            tilemap.draw(-cameraX, -cameraY);
            dor.draw(doorX, doorY);
            player.draw(playerX, playerY, false, false, recolor);
            
            oldX = cameraX;
            oldY = cameraY;
            
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
                    Display::print("What does that button do?");
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
            tileX = (cameraX + playerX + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + playerY - 8 + playerHeight) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
    
            if( tile == Hack ){
                // Eventually play a Hack tone here, draw hack when actually hacking
                // Play hackable notification 
                if( Buttons::aBtn() ){
                    SeqHack::init();
                    SeqHack::shuffle(4);
                    prevState = State::EXPLORE;
                    state = State::SEQUENCE;
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
            tileY = (cameraY + playerY + 8 + playerHeight) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
            
            if(tile==Door){
                tilemap.set(roboIntro[0], roboIntro[1], roboIntro+2);
                for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
                    tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
                }
                cameraX = -54;
                cameraY = 68;
                getTile = roboIntroEnum;
                dor.play(door, Door::locked);
                doorLocked = true;
                state = State::HALLWAY;
            }
            
            doorX = -cameraX+48;
            doorY = -cameraY;
            
            Display::setColor(7);
            break;
        case HALLWAY:
        
            tilemap.draw(-cameraX, -cameraY);
            dor.draw(doorX, doorY);
            botField.draw(-cameraX+88, -cameraY+88);
            
            player.draw(playerX, playerY, false, false, recolor);
             
            oldX = cameraX;
            oldY = cameraY;
            
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
                    Display::print("How do I get the button?");
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
            tileX = (cameraX + playerX + PROJ_TILE_W/2) / PROJ_TILE_W;
            tileY = (cameraY + playerY - 8 + playerHeight) / PROJ_TILE_H;
            tile = getTile(tileX, tileY);
    
            if( tile == Hack ){
                // Eventually play a Hack tone here, draw hack when actually hacking
                // Play hackable notification 
                if( Buttons::aBtn() ){
                    RoboHack::init(4);
                    prevState = State::HALLWAY;
                    
                    tilemap.set(minibotfield[0], minibotfield[1], minibotfield+2);
                    for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
                        tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
                    }
                    getTile = minibotfieldEnum;
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
    
            
            doorX = -cameraX+208;
            doorY = -cameraY+32;
            
            
            
            
            Display::setColor(7);
        break;
        }
    }
    return 0;
}
