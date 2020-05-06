
#include <Pokitto.h>
#include "sprites.h"
#include <tasui>
#include <puits_UltimateUtopia.h>
#include "src/Minigames/Sequence.hpp"
#include "src/ButtonMaps.h"
#include "src/structs.h"
#include <LibLog>

namespace Sequence {
    using Pokitto::UI;
    void SeqHack::shuffle(int size) {
        // init hacking sequence variables.
        seqSize = size;
        // srand((unsigned int) time (NULL));
        
        for(int i = 0; i < seqSize; ++i){
            int r = rand()%7;
            switch(r){
                case 0:
                    order[i] = B_A;
                    break;
                case 1:
                    order[i] = B_B;
                    break;
                case 2:
                    order[i] = B_C;
                    break;
                case 3:
                    order[i] = B_UP;
                    break;
                case 4:
                    order[i] = B_RIGHT;
                    break;
                case 5:
                    order[i] = B_DOWN;
                    break;
                case 6:
                    order[i] = B_LEFT;
                    break;
            }
        }
        
        // reset order status
        for(int i = 0; i < seqSize; ++i){
            orderStatus[i] = false;   
        }
    }
    
    int SeqHack::getSelect(){
        return select;
    }
    
    void SeqHack::init(int dt){
        icons.play(hackIcons, HackIcons::bUp);
        tLevel.play(threatLevel, ThreatLevel::zero);
        sent.play(sentinal, Sentinal::scanning);
        tLevel.play(threatLevel, ThreatLevel::zero);
        
        select = 0;
        threat = 0;
        detectTime = dt;
        detectSpeed = dt;
        
        end = false;
        seqState = SeqState::READY;
        
        UI::setTilesetImage(puits::UltimateUtopia::tileSet);
        // NEW - Show the Tilemap, the Sprites, then the UI.
        UI::showTileMapSpritesUI();
    }
    
    void SeqHack::update() {
        using Pokitto::Buttons;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;

        switch(seqState){
            case READY:
                if(buttonsJustPressed == B_C){
                    seqState = SeqState::RUNNING;
                }
                break;
            case RUNNING:
            
                if (buttonsJustPressed & order[select] ){
                    orderStatus[select] = true;
                    select++;
                } else if (buttonsJustPressed != 0) {
                    threat++;
                }
                
                detectSpeed--;
                if(detectSpeed < 0){
                    detectSpeed = detectTime;
                    threat++;
                }
                
                switch(threat){
                    case 1:
                        tLevel.play(threatLevel, ThreatLevel::one);
                        break;
                    case 2:
                        tLevel.play(threatLevel, ThreatLevel::two);
                        break;
                    case 3:
                        tLevel.play(threatLevel, ThreatLevel::three);
                        break;
                    case 4:
                        tLevel.play(threatLevel, ThreatLevel::four);
                        break;
                    case 5:
                        tLevel.play(threatLevel, ThreatLevel::five);
                        sent.play(sentinal, Sentinal::warning);
                        threat++;
                        break;
                }
                
                if(select == seqSize){
                    seqState = SeqState::COMPLETE;
                }
                break;
            case COMPLETE:
                if(buttonsJustPressed == B_B){
                    end = true;
                }
                break;
        }
    }
    
    bool SeqHack::complete(){
        return end;
    }
    
    bool SeqHack::fail(){
        return threat > 5;
    }
    
    void SeqHack::drawUI(){
        UI::clear();
        UI::drawBox(1, 1, 32, 3);
        UI::setCursorBoundingBox(2, 2, 31, 1);
        UI::setCursor(2, 2);
        UI::setCursorDelta(UIVariants::standard);
    }
    
    // TODO: Don't worry too much about ugly prog bar. Will replace with TASUI Gauge
    void SeqHack::render(){
        switch(seqState){
            case READY:
                // Display::print("> Press C to hack!");
                SeqHack::drawUI();
                UI::printText("> C to hack door terminal!");
            break;
            case RUNNING:
                UI::clear();
                for(int x = 0; x < seqSize; ++x){
                    switch(order[x]){
                        case B_A:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::aDown : HackIcons::aUp));
                            break;
                        case B_B:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::bDown : HackIcons::bUp));
                            break;
                        case B_C:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::cDown : HackIcons::cUp));
                            break;
                        case B_UP:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::dUpDown : HackIcons::dUpUp));
                            break;
                        case B_DOWN:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::dDownDown : HackIcons::dDownUp));
                            break;
                        case B_LEFT:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::dLeftDown : HackIcons::dLeftUp));
                            break;
                        case B_RIGHT:
                            icons.play(hackIcons, (orderStatus[x] ? HackIcons::dRightDown : HackIcons::dRightUp));
                            break;
                    }
                    icons.draw(8 + x * 16, 32);
                }
                sent.draw(200, 50);
                tLevel.draw(200, 70);
                
                UI::drawGauge(1, 34, 4, select, seqSize);
                
            break;
            
            case COMPLETE:
                UI::drawGauge(1, 34, 4, select, seqSize);
                SeqHack::drawUI();
                UI::printText("> Press B to exit.");
            break;
        }
        
    }
}