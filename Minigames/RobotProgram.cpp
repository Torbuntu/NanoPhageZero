
#include <Pokitto.h>
#include <Tilemap.hpp>
#include <ButtonMaps.h>
#include "sprites.h"
#include "Minigames/RobotProgram.hpp"
#include "maps.h"

namespace RobotProgram{
    
    void RoboHack::restart(){
        roboX = 0;
        roboY = 0;
        step = 0;
        
        keyX = rand()%5;
        if(keyX == 0) keyX++;
        
        keyY = rand()%4;
        if(keyY == 0) keyY++;
        
        btnX = rand()%5;
        if(btnX == 0 || btnX == keyX) btnX++;
        if(btnX == 6){
            if(btnX == keyX){
                btnX -= 2;
            }else{
                btnX--;
            }
        }
        btnY = rand()%4;
        if(btnY == 0 || btnY == keyY) btnY++;
        if(btnY == 5){
            if(btnY == keyY){
                btnY -= 2;
            }else{
                btnY--;
            }
        }
        
        hasKey = false;
        unlocked = false;
        
        for(int i = 0; i < length; ++i){
            program[i] = 10;
        }
    }

    void RoboHack::init(int len){
        icons.play(hackIcons, HackIcons::aUp);
        robo.play(robot, Robot::idle);
        keyIcon.play(key, Key::idle);
        buttonIcon.play(button, Button::idle);
        
        length = len;
        end = false;
        speed = 15;
        
        //0 = 14, 1 = 11, suburb+2 = mapPixelData
        tilemap.set(minibotfield[0], minibotfield[1], minibotfield+2);
        for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
            tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
        }
        
        RoboHack::restart();
        roboState = RoboState::READY;
    }
    
    void RoboHack::update(){
        using Pokitto::Buttons;
        using Pokitto::Display;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        switch(roboState){
            case READY:
                Display::print("> Press C to program robot");
                if( buttonsJustPressed == B_C ){
                    roboState = RoboState::PROGRAMMING;
                }
            
            break;
            case PROGRAMMING:
                Display::print("> Programming...");
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
                Display::print("> Running...");
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
                Display::print("> C to restart. ");
                if(unlocked){
                    Display::print("B to exit.");
                }
                if( buttonsJustPressed == B_B ){
                    if(unlocked){
                        end = true;
                    }
                }
                
                if( buttonsJustPressed == B_C){
                    RoboHack::restart();
                    roboState = RoboState::PROGRAMMING;
                }
            break;
        }
        
        
    }
    
    void RoboHack::render(){
        using Pokitto::Display;
        for(int i = 0; i < length; ++i){
            switch(program[i]){
                case B_A:
                    icons.play(hackIcons, HackIcons::aUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_B:
                    icons.play(hackIcons, HackIcons::bUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_C:
                    icons.play(hackIcons, HackIcons::cUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_UP:
                    icons.play(hackIcons, HackIcons::dUpUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_DOWN:
                    icons.play(hackIcons,  HackIcons::dDownUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_LEFT:
                    icons.play(hackIcons, HackIcons::dLeftUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_RIGHT:
                    icons.play(hackIcons,  HackIcons::dRightUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                default:
                    icons.play(hackIcons, HackIcons::empty);
                    icons.draw(8 + i * 16, 16);
            }
        }
        if(roboState == RoboState::RUNNING){
            robo.draw(8+step*16, 32);
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
        
        Display::print(0, 160, "A pickup, B use. C skip.");
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
    
}