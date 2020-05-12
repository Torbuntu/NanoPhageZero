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
        
        dialogA[0] = "<Cuber CEO> I've been expecting you.";
        dialogA[1] = "You don't think that Green Scrap at the facility was accidental, do you?";
        dialogA[2] = "I sent the NanoPhage to the facility. It was supposed to consume everybody there.";
        dialogA[3] = "But I see it didn't completely consume you. Just... your arm and your eyes... remain.";
        dialogA[4] = "That doesn't matter now. You were still driven by the NanoPhage to come home.";
        dialogA[5] = "I know you feel it. The hunger to be superior. To be more than just a rotting meat bag.";
        dialogA[6] = "I was human once. But I've since been upgraded. As you can see, I'm now far superior.";
        dialogA[7] = "You have a choice. Join me, and be superior. Or defeat me, and receive the cure.";
        
        
        dialogB[0] = "<Cuber CEO> You seriously think you can defeat me with moves like that? Ha ha ha!";
        dialogB[1] = "You're going to have to try much harder than that.";
        
        dialogC[0] = "<Cyber CEO> What... How is this possible?? There is no way you're this strong...";
        dialogC[1] = "A glitch, there must be a glitch... but... ";
        dialogC[2] = "<ANGRY GROANING> This is not possible! I made the NanoPhage, it can't turn against...";
        dialogC[3] = "<CYBER CEO BEGINS TO FADE>";
        dialogC[4] = "This isn't over!! <ZZZAAAAAPPP>!!!";
        
        brutState = BrutState::DIALOG_A;
    }
    
    void BossFight::update(){
        using Pokitto::Buttons;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        switch(brutState){
            case DIALOG_A:
                if(buttonsJustPressed == B_A && dialogPos < 8) dialogPos++;
                if(dialogPos == 8) {
                    brutState = CHOICE;
                    dialogPos = 0;
                }
            break;
            case DIALOG_B:
                drawUI();
                if(buttonsJustPressed == B_A && dialogPos < 2) dialogPos++;
                if(dialogPos == 2){
                    dialogPos = 0;
                    brutState = PAUSE;
                }
            break;
            case DIALOG_C:
                if(buttonsJustPressed == B_A && dialogPos < 5) dialogPos++;
                if(dialogPos == 5){
                    dialogPos = 0;
                    brutState = COMPLETE;
                }
            break;
            case READY:
                
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
                    brutState = DIALOG_B;
                }
                
                if(bruteProgress >= bruteFill){
                    victory = true;
                    brutState = DIALOG_C;
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
            case DIALOG_A:
                drawUI();
                UI::printText(dialogA[dialogPos]);
            break;
            case DIALOG_B:
                drawUI();
                UI::printText(dialogB[dialogPos]);
            break;
            case DIALOG_C:
                drawUI();
                UI::printText(dialogC[dialogPos]);
            break;
            case CHOICE:
                drawUI();
                UI::printText("> B - join cyber race\n> C - battle Cyber CEO");
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
    
    bool BossFight::isPaused(){
        return brutState == PAUSE;
    }
}