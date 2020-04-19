#pragma once
#include <Pokitto.h>
#include "sprites.h"

namespace Bruteforce {
    
    enum BrutState{
        READY, RUNNING, COMPLETE  
    };
    
    class BruteHack{
        public:
            static void init();
            
            static void update();
            
            static void render();
            
            static bool complete();
            
            static bool fail();
        
        private:
            static inline int select = 0;
        
            static inline int bruteCount = 0;
            static inline int bruteSelect;
            static inline int bruteProgress = 0;
            static inline int enemyProgress = 0;
            static inline int enemyTimer = 0;
            static inline int enemySpeed = 0;
        
            // init button state mechanism
            static inline int buttonsPreviousState = 0;
            
            static inline int buttonsJustPressed = 0;
            
            static inline BrutState brutState = BrutState::READY;
            
            static inline bool end;
            
            static inline Sprite icons, pBar, pFill, eFill, virus;
    };
}