#pragma once
#include <Pokitto.h>
#include "sprites.h"

namespace BossBattle {
    
    enum BrutState{
        READY, RUNNING, CHOICE, PAUSE, COMPLETE  
    };
    
    class BossFight{
        public:
            static void init(int count);
            
            static bool running();
            
            static void resume();
            
            static void update();
            
            static void render();
            
            static bool complete();
            
            static bool fail();
            
            static void setSelect();
            
            static bool getVictory();
            
            static bool loop();
            
            static void endLoop();
        private:
            static void drawUI();
        
            static inline int select = 0;
            
            static inline int dialogPos = 0;
        
            static inline int bruteCount = 0;
            static inline int bruteSelect;
            static inline int bruteProgress = 0;
            static inline int enemyProgress = 0;
            static inline int enemyTimer = 0;
            static inline int enemySpeed = 0;
            static inline int bruteFill = 100;
        
            // init button state mechanism
            static inline int buttonsPreviousState = 0;
            
            static inline int buttonsJustPressed = 0;
            
            static inline BrutState brutState = BrutState::READY;
            
            static inline bool end, victory, loopcheck = true;
            
            static inline Sprite icons;
            
            static inline char* dialog[5];
    };
}