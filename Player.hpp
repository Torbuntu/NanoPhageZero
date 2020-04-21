#pragma once

#include <sprites.h>

namespace Player{
    class PlayerManager{
        public:
            static void init();
            static void update(int& x, int& y);
            static void render();
            
            static int getX();
            static int getY();
            static int getW();
            static int getH();
        
        private:
        
            static inline Sprite player;
        
            static inline int x, y, w, h, speed;
    };
}