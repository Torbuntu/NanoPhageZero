#include "src/Minigames/Bruteforce.hpp"
#include "src/ButtonMaps.h"
#include <Pokitto.h>
#include "sprites.h"
#include <tasui>
#include <puits_UltimateUtopia.h>

namespace Bruteforce {
    void BruteHack::init(int count){
        
        bruteCount = count;
        select = 0;
        bruteSelect = B_A;
        bruteProgress = 0;
        enemyProgress = 1;
        enemySpeed = 15;
        enemyTimer = 0;
        
        end = false;
        
        brutState = BrutState::READY;
        
        icons.play(hackIcons, HackIcons::aUp);
        virus.play(enemyVirus, EnemyVirus::play);
        
        srand((unsigned int) time (NULL));
    }
    
    void BruteHack::update(){
        using Pokitto::Buttons;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        switch(brutState){
            case READY:
                if(buttonsJustPressed == B_C){
                    brutState = BrutState::RUNNING;
                }
            break;
            case RUNNING:
                if(buttonsJustPressed & bruteSelect){
                    select++;
                    bruteProgress++;
                }
                if( select == bruteCount ){
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
                    select = 0;
                }
                
                enemyTimer++;
                if(enemyTimer == enemySpeed){
                    enemyTimer = 0;
                    if(enemyProgress < bruteFill){
                        enemyProgress++;
                    }
                }
                if(bruteProgress >= bruteFill){
                    brutState = BrutState::COMPLETE;   
                }
            break;
            case COMPLETE:
                if(buttonsJustPressed == B_B){
                    end = true;
                }
            break;
        }
    }
    
    void BruteHack::render(){
        using Pokitto::Display;
        using Pokitto::UI;
        
        switch(brutState){
            case READY:
                Display::print("> Press C to hack!");
            break;
            case RUNNING:
                // icon to press
                icons.draw(8, 32);
                
                virus.draw(190, 130);
                
                // player bar
                UI::drawGauge(1, 34, 2, bruteProgress, bruteFill);
                
                // virus bar
                UI::drawGauge(1, 34, 20, enemyProgress, bruteFill);
            break;
            case COMPLETE:
                // player bar
                UI::drawGauge(1, 34, 2, bruteProgress, bruteFill);
                
                // virus bar
                UI::drawGauge(1, 34, 20, enemyProgress, bruteFill);
                Display::print("> B exit.");
            break;
        }
           
    }
    
    bool BruteHack::complete(){
        return end;
    }
    
    // TODO: Actually implement failure.
    bool BruteHack::fail(){
        return enemyProgress >= 30;
    }
}