#include "Minigames/Sequence.hpp"
#include "ButtonMaps.h"
#include <Pokitto.h>
#include "sprites.h"


namespace Sequence {
    void SeqHack::shuffle(int size) {
        // init hacking sequence variables.
        seqSize = size;
        srand((unsigned int) time (NULL));
        
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
    
    void SeqHack::init(){
        icons.play(hackIcons, HackIcons::bUp);
        tLevel.play(threatLevel, ThreatLevel::zero);
        sent.play(sentinal, Sentinal::scanning);
        pBar.play(progBar, ProgBar::start);
        pFill.play(progFill, ProgFill::play);
        tLevel.play(threatLevel, ThreatLevel::zero);
        
        select = 0;
        threat = 0;
        
        end = false;
        
        seqState = SeqState::READY;
        
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
                if(buttonsJustPressed == B_C){
                    end = true;
                }
                break;
        }
    }
    
    bool SeqHack::complete(){
        return end;
    }
    
    bool SeqHack::fail(){
        return threat > 8;
    }
    
    // TODO: Don't worry too much about ugly prog bar. Will replace with TASUI Gauge
    void SeqHack::render(){
        using Pokitto::Display;
        
        switch(seqState){
            case READY:
                Display::print("> Press C to hack!");
            break;
            case RUNNING:
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
                pBar.draw(10, 10);
                for(int i = 0; i < select * 2; ++i){
                    pFill.draw(14 + i * 15, 14);
                }
                sent.draw(200, 50);
                tLevel.draw(200, 70);
            break;
            
            case COMPLETE:
                Display::print("> Press C to end.");
            break;
        }
        
    }
}