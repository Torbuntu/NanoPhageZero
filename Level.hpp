#pragma once

#include <Pokitto.h>
#include <Tilemap.hpp>

namespace Level{
    class LevelManager{
        public:
            static void setMap(const uint8_t* map);
            
            static void init();
            
            static void update();
            
            static void render(int x, int y);
            
        private:
        
            static inline Tilemap tilemap;
        
    };
}