#pragma once

#include <Pokitto.h>
#include <Tilemap.hpp>
#include <sprites.h>
#include <maps.h>

namespace Level{
    class LevelManager{
        public:
            using GetEnum = MapEnum (*)(uint32_t, uint32_t);
            
            static void setMap(const uint8_t* map, GetEnum num);
            
            static void setMap();
            
            static void addLvl();
            
            static void setLvl(int lvl);
            
            static int getLvl();
            
            static void setDroneState(bool active);
            
            static bool checkDrone(int playerX, int playerY);
            
            static void init();
            
            static void update();
            
            static void render(int x, int y);
            
        private:
        
            static inline Tilemap tilemap;
            
            static inline int level, bfX, bfY, dX, dY;
            
            static inline Sprite botField, drone;
            
            static inline bool hasDrone, hasBotfield, droneActive;
        
    };
}