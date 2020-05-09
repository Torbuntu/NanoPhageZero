
#include <Pokitto.h>
#include <Tilemap.hpp>
#include <src/ButtonMaps.h>
#include "src/Minigames/RobotProgram.hpp"
#include "sprites.h"
#include <tasui>
#include <puits_UltimateUtopia.h>
#include "src/structs.h"

namespace RobotProgram{
    using Pokitto::UI;
    
    void RoboHack::drawUI(){
        UI::clear();
        UI::drawBox(1, 0, 35, 2);
        UI::setCursorBoundingBox(2, 1, 34, 1);
        UI::setCursor(1, 1);
        UI::setCursorDelta(UIVariants::standard);
    }
    
    void RoboHack::restart(){
        roboX = 0;
        roboY = 0;
        step = 0;
        // srand((unsigned int) time (NULL));
        
        keyX = rand()%6;
        if(keyX == 0) keyX++;
        
        keyY = rand()%5;
        if(keyY == 0) keyY++;
        
        btnX = rand()%6;
        if(btnX == 0 || btnX == keyX) btnX++;
        if(btnX >= 5){
            if(btnX == keyX){
                btnX -= 2;
            }else{
                btnX--;
            }
        }
        btnY = rand()%5;
        if(btnY == 0 || btnY == keyY) btnY++;
        if(btnY >= 4){
            if(btnY == keyY){
                btnY -= 2;
            }else{
                btnY--;
            }
        }
        
        vX = rand()%6;
        if(vX == 0 || vX == keyX) vX++;
        if(vX >= 5){
            if(vX == keyX){
                vX -= 2;
            }else{
                vX--;
            }
        }
        vY = rand()%5;
        if(vY == 0 || vY == keyY) vY++;
        if(vY >= 4){
            if(vY == keyY){
                vY -= 2;
            }else{
                vY--;
            }
        }
        
        vD = rand()%2;
        vDInit = vD;
        vS = 1;
        
        vInitX = vX;
        vInitY = vY;
        
        
        hasKey = false;
        unlocked = false;
        
        keyInitX = keyX;
        keyInitY = keyY;
        
        for(int i = 0; i < length; ++i){
            program[i] = 10;
        }
        roboState = RoboState::READY;
    }

    void RoboHack::init(int len){
        icons.play(hackIcons, HackIcons::aUp);
        robo.play(robot, Robot::idle);
        keyIcon.play(key, Key::idle);
        buttonIcon.play(button, Button::idle);
        virus.play(enemyVirus, EnemyVirus::play);
        
        length = len;
        end = false;
        speed = 15;
        iconOffset = 7;

        RoboHack::restart();
    }
    
    void RoboHack::update(){
        using Pokitto::Buttons;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        if(!intro && roboState == RoboState::RUNNING){
            if(speed == 1){
                if(vD == 0){
                    vX += vS;
                    if(vX ==5 || vX == 0){
                        vS = -vS;
                    }
                }else {
                    vY += vS;
                    if(vY == 4 || vY == 0){
                        vS = -vS;
                    }
                }
            }

            if(vX == keyX && vY == keyY){
                if(vD == 0){
                    if(vS > 0){
                        keyX++;
                    }else {
                        keyX--;
                    }
                }else {
                    if(vS > 0){
                        keyY++;
                    }else {
                        keyY--;
                    }
                }
            }
            
            if(vX == roboX && vY == roboY){
                if(vD == 0){
                    if(vS > 0){
                        roboX++;
                    }else {
                        roboX--;
                    }
                }else {
                    if(vS > 0){
                        roboY++;
                    }else {
                        roboY--;
                    }
                }
            }
        }
        
        switch(roboState){
            case READY:
                RoboHack::drawUI();
                UI::printText("> Press C to program robot");
                // Display::print("> Press C to program robot");
                if( buttonsJustPressed == B_C ){
                    roboState = RoboState::PROGRAMMING;
                }
            
            break;
            case PROGRAMMING:
                RoboHack::drawUI();
                UI::printText("> Programming...");
                if( buttonsJustPressed != 0 && step < length ){
                    program[step] = buttonsJustPressed;
                    step++;
                }
                
                if(step == length){
                    step = 0;
                    roboState = RoboState::RUNNING;
                }
            break;
            case RUNNING:
                RoboHack::drawUI();
                UI::printText("> Running...");
                speed--;
                
                if(speed == 0){
                    speed = 15;
                    if(step < length){
                        switch(program[step]){
                            case B_A:
                                if(roboX == keyX && roboY == keyY){
                                    hasKey = true;
                                }
                            break;
                            case B_B:
                                if(roboX == btnX && roboY == btnY){
                                    unlocked = true;
                                }else{
                                    keyX = roboX;
                                    keyY = roboY;
                                    hasKey = false;
                                }
                            break;
                            case B_LEFT:
                                roboX--;
                                if(roboX < 0){
                                    roboX = 0;
                                }
                            break;
                            case B_RIGHT:
                                roboX++;
                                if(roboX > 5){
                                    roboX = 5;
                                }
                            break;
                            case B_UP:
                                roboY--;
                                if(roboY < 0){
                                    roboY = 0;
                                }
                            break;
                            case B_DOWN:
                                roboY++;
                                if(roboY > 4){
                                    roboY = 4;
                                }
                            break;
                        }
                        step++;
                    }
                }
                
                if(step == length){
                    step = 0;
                    roboState = RoboState::COMPLETE;
                }
            
            break;
            case COMPLETE:
                RoboHack::drawUI();
                UI::printText("> C restart. A new. ");
                if(unlocked){
                    UI::printText("B exit.");
                }
                if( buttonsJustPressed == B_B ){
                    if(unlocked){
                        end = true;
                    }
                }
                if( buttonsJustPressed == B_A){
                    RoboHack::restart();
                }
                
                if( buttonsJustPressed == B_C){
                    // RoboHack::restart();
                    roboX = 0;
                    roboY = 0;
                    step = 0;
                    
                    vX = 5;
                    vY = 4;
                    
                    keyX = keyInitX;
                    keyY = keyInitY;
                    
                    vX = vInitX;
                    vY = vInitY;
                    
                    vD = vDInit;
                    
                    hasKey = false;
                    unlocked = false;
                    roboState = RoboState::PROGRAMMING;
                }
            break;
        }
    }
    
    void RoboHack::render(){
        for(int i = 0; i < length; ++i){
            switch(program[i]){
                case B_A:
                    icons.play(hackIcons, HackIcons::aUp);
                    break;
                case B_B:
                    icons.play(hackIcons, HackIcons::bUp);
                    break;
                case B_C:
                    icons.play(hackIcons, HackIcons::cUp);
                    break;
                case B_UP:
                    icons.play(hackIcons, HackIcons::dUpUp);
                    break;
                case B_DOWN:
                    icons.play(hackIcons,  HackIcons::dDownUp);
                    break;
                case B_LEFT:
                    icons.play(hackIcons, HackIcons::dLeftUp);
                    break;
                case B_RIGHT:
                    icons.play(hackIcons,  HackIcons::dRightUp);
                    break;
                default:
                    icons.play(hackIcons, HackIcons::empty);
            }
            icons.draw(iconOffset + i * 16, 18);
        }
        if(roboState == RoboState::RUNNING){
            robo.draw(iconOffset+step*16, 32);
        }
        
        if(!unlocked){
            if(hasKey){
                keyIcon.draw(16, 128);
            }else{
                keyIcon.draw(64 + keyX * 16, 48 + keyY * 16);
            }
        }
        
        buttonIcon.draw(64 + btnX * 16, 48 + btnY * 16);
        robo.draw(64 + roboX * 16, 48 + roboY * 16);
        if(!intro){
            virus.draw(64 + vX * 16, 48 + vY * 16);
        }
        
        UI::drawBox(1, 24, 35, 26);
        UI::setCursorBoundingBox(2, 2, 34, 25);
        UI::setCursor(2, 25);
        UI::setCursorDelta(UIVariants::standard);
        UI::printText("A pickup, B use. C skip.");
    }
    
    bool RoboHack::complete(){
        return end;   
    }
    
    int RoboHack::getRobotX(){
        return roboX;
    }
    
    int RoboHack::getRobotY(){
        return roboY;
    }
    
    void RoboHack::setRobotX(int x){
        roboX = x;
    }
    
    void RoboHack::setRobotY(int y){
        roboY = y;
    }
    
    void RoboHack::setIntro(bool tut){
        intro = tut;
    }
    
}