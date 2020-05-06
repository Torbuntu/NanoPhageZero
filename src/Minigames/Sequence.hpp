#pragma once
#include <Pokitto.h>
#include "sprites.h"

namespace Sequence {
    
    enum SeqState {
        READY, RUNNING, COMPLETE
    };
    
    class SeqHack {
        public:
        
            // initializes a new random sequence
            static void shuffle(int size);
            
            static void init(int dt);
            
            static void update();
            
            static void render();
            
            static bool complete();
            
            static bool fail();
            
            static int getSelect();
            
        private:
        
            static void drawUI();
            
            static inline SeqState seqState = SeqState::READY;
            
            // Size of the sequence
            static inline int seqSize; 
            
            static inline int threat;
            
            static inline int select = 0, detectTime = 10, detectSpeed;
                
            // the sequence order
            static inline int order[13];
            
            // status of current sequence
            static inline bool orderStatus[13];
            
            static inline bool end = false;
            
            // init button state mechanism
            static inline int buttonsPreviousState = 0, buttonsJustPressed = 0;
            
            static inline Sprite icons, tLevel, sent, virus;
    };
}