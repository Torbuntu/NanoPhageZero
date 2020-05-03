#include <Pokitto.h>
#include <sprites.h>
#include <src/Player.hpp>

namespace Player{
    void PlayerManager::init(){
        player.play(hero, Hero::walkSouth);
        
        w = 16;
        h = 16;
        x = LCDWIDTH/2 - w/2;
        y = LCDHEIGHT/2 - h/2;
        
        speed = 1;
    }
    
    int PlayerManager::getX(){
        return x;
    }
    int PlayerManager::getY(){
        return y;
    }
    int PlayerManager::getW(){
        return w;
    }
    int PlayerManager::getH(){
        return h;
    }
    
    void PlayerManager::update(int& cameraX, int& cameraY){
        using Pokitto::Buttons;
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
    }
    
    void PlayerManager::render(){
        player.draw(x, y, false, false, 0);
    }
}