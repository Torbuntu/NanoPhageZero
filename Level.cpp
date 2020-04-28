#include "Level.hpp"
#include <Tilemap.hpp>
#include <maps.h>
#include <sprites.h>

namespace Level{
    using GetEnum = MapEnum (*)(uint32_t, uint32_t);
    void LevelManager::setMap(const uint8_t* map, GetEnum num){
        tilemap.set(map[0], map[1], map+2);
        for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
            tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
        }
        hasBotfield = false;
        hasDrone = false;
        
        drone.play(securityDrone, SecurityDrone::active);
        for(int x = 0; x < 14; ++x){
            for(int y = 0; y < 11; ++y){
                auto tile = num(x,y);
                if( tile == Drone ){
                    dX = x * POK_TILE_W;
                    dY = y * POK_TILE_H;
                    hasDrone = true;
                    droneActive = true;
                }
                if(tile == Botfield ){
                    bfX = x * POK_TILE_W + 8;
                    bfY = y * POK_TILE_H + 8;
                    hasBotfield = true;
                }
            }
        }
    }

    void LevelManager::setMap(){
        switch(level){
            case 0:
                LevelManager::setMap(lobby, lobbyEnum);
                break;
            case 1:
                LevelManager::setMap(lvl1, lvl1Enum);
                break;
        }
    }
    
    void LevelManager::addLvl(){
        level++;
    }
    
    void LevelManager::setLvl(int lvl){
        level = lvl;
    }
    
    int LevelManager::getLvl(){
        return level;
    }
    
    void LevelManager::setDroneState(bool active){
        droneActive = active;
        if(active){
            drone.play(securityDrone, SecurityDrone::active);
        }else {
            drone.play(securityDrone, SecurityDrone::inactive);
        }
    }
    
    bool LevelManager::checkDrone(int x, int y){
        if(droneActive){
            return x == dX || y == dY;
        }
        
        return false;
    }
    
    void LevelManager::init(){
        tilemap.fillOutOfBounds = 255;
        level = 0;
        botField.play(minibotField, MinibotField::idle);
        drone.play(securityDrone, SecurityDrone::active);
    }
    
    void LevelManager::update(){
        
    }
    
    void LevelManager::render(int x, int y){
        tilemap.draw(x, y);
        
        if(hasDrone) drone.draw(x + dX, y + dY);
        if(hasBotfield) botField.draw(x + bfX, y + bfY);
    }
    
}