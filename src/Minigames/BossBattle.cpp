#include "src/Minigames/BossBattle.hpp"
#include "src/ButtonMaps.h"
#include <Pokitto.h>
#include "sprites.h"
#include <tasui>
#include <puits_UltimateUtopia.h>
#include "src/structs.h"

namespace BossBattle {
    using Pokitto::UI;
    void BossFight::drawUI(){
        UI::clear();
        UI::drawBox(1, 24, 35, 24 + 4);
        UI::setCursorBoundingBox(1 + 1, 24 + 1, 35 - 1, 24 + 4 - 1);
        UI::setCursor(1 + 1, 24 + 1);
        UI::setCursorDelta(UIVariants::standard);
    }
    
    void BossFight::init(int count){
        icons.play(hackIcons, HackIcons::aUp);
        
        bruteCount = count;
        select = 0;
        setSelect();
        bruteProgress = 0;
        enemyProgress = 1;
        enemySpeed = 5;
        enemyTimer = 0;
        
        end = false;
        victory = false;
        
        dialog[0] = "I've been expecting you.";
        dialog[1] = "You don't think that Green Scrap at the facility was accidental, right?";
        dialog[2] = "It was supposed to completely consume you. But here you are... unconsumed.";
        dialog[3] = "I was human once. But I've since been upgraded. As you can see, I'm now far superior.";
        dialog[4] = "You have a choice. Join me, and be superior. Or defeat me, and receive the cure.";
        
        brutState = BrutState::READY;
    }
    
    void BossFight::update(){
        using Pokitto::Buttons;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        switch(brutState){
            case READY:
                if(buttonsJustPressed == B_A && dialogPos < 5) dialogPos++;
                if(dialogPos == 5) brutState = CHOICE;
            break;
            case CHOICE:
                if(buttonsJustPressed == B_B) brutState = COMPLETE;
                if(buttonsJustPressed == B_C) brutState = RUNNING;
            break;
            case RUNNING:
                if(buttonsJustPressed & bruteSelect){
                    select++;
                    bruteProgress++;
                }
                if( select == bruteCount ){
                    setSelect();
                    select = 0;
                }
                
                enemyTimer++;
                if(enemyTimer == enemySpeed){
                    enemyTimer = 0;
                    if(enemyProgress < bruteFill){
                        enemyProgress++;
                    }
                }
                
                if( enemyProgress >= bruteFill ){
                    brutState = PAUSE;
                }
                
                if(bruteProgress >= bruteFill){
                    victory = true;
                    brutState = BrutState::COMPLETE;   
                }
            break;
            case PAUSE:
            
            break;
            case COMPLETE:
                end = true;
            break;
        }
    }
    
    void BossFight::setSelect(){
        int r = rand()%7;
        switch(r){
            case 0:
                bruteSelect = B_A;
                icons.play(hackIcons, HackIcons::aUp);
                break;
            case 1:
                bruteSelect = B_B;
                icons.play(hackIcons, HackIcons::bUp);
                break;
            case 2:
                bruteSelect = B_C;
                icons.play(hackIcons, HackIcons::cUp);
                break;
            case 3:
                bruteSelect = B_UP;
                icons.play(hackIcons, HackIcons::dUpUp);
                break;
            case 4:
                bruteSelect = B_RIGHT;
                icons.play(hackIcons, HackIcons::dRightUp);
                break;
            case 5:
                bruteSelect = B_DOWN;
                icons.play(hackIcons, HackIcons::dDownUp);
                break;
            case 6:
                bruteSelect = B_LEFT;
                icons.play(hackIcons, HackIcons::dLeftUp);
                break;
        }
    }
    
    void BossFight::render(){
        using Pokitto::UI;
        
        switch(brutState){
            case READY:
                drawUI();
                UI::printText(dialog[dialogPos]);
            break;
            case CHOICE:
                drawUI();
                UI::printText("> B - join cyber race\n> C - battle cyborg CEO");
            break;
            case RUNNING:
                UI::clear();
                // icon to press
                icons.draw(8, 40);
                
                // player bar
                UI::drawGauge(1, 34, 5, bruteProgress, bruteFill);
                
                // virus bar
                UI::drawGauge(1, 34, 20, enemyProgress, bruteFill);
            break;
            case PAUSE:
                UI::clear();
            break;
            case COMPLETE:
                UI::clear();
                break;
        }
           
    }
    
    void BossFight::resume(){
        enemyProgress = 0;
        brutState = RUNNING;
    }
    
    bool BossFight::complete(){
        return end;
    }
    
    // TODO: Actually implement failure.
    bool BossFight::fail(){
        return enemyProgress >= 30;
    }
    
    bool BossFight::running(){
        return brutState == RUNNING;
    }
    
    bool BossFight::getVictory(){
        return victory;
    }
    
    void BossFight::endLoop(){
        loopcheck = false;
    }
    
    bool BossFight::loop(){
        return loopcheck;
    }
}