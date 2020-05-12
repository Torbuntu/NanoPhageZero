#pragma once
#include <Pokitto.h>
#include "sprites.h"

namespace BossBattle {
    
    enum BrutState{
        DIALOG_A, DIALOG_B, DIALOG_C, READY, RUNNING, CHOICE, PAUSE, COMPLETE  
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
            
            static bool isPaused();

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
            
            static inline bool end, victory;
            
            static inline Sprite icons;
            
            static inline char* dialogA[8];
            static inline char* dialogB[2];
            static inline char* dialogC[5];
    };
}