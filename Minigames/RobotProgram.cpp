
#include <Pokitto.h>
#include <ButtonMaps.h>
#include "sprites.h"
#include "Minigames/RobotProgram.hpp"
#include "maps.h"

namespace RobotProgram{
    
    void RoboHack::init(int len, int robX, int robY){
        icons.play(hackIcons, HackIcons::aUp);
        robo.play(robot, Robot::idle);
        length = len;
        roboX = robX;
        roboY = robY;
        step = 0;
        inventory = 0;
        end = false;
        
        speed = 15;
        
        roboState = RoboState::READY;
        
        for(int i = 0; i < length; ++i){
            program[i] = 10;
        }
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
                            case B_LEFT:
                                roboX -= 16;
                            break;
                            case B_RIGHT:
                                roboX += 16;
                            break;
                            case B_UP:
                                roboY -= 16;
                            break;
                            case B_DOWN:
                                roboY += 16;
                            break;
                        }
                        step++;
                    }
                }
                
                if(step == length){
                    roboState = RoboState::COMPLETE;
                }
            
            break;
            case COMPLETE:
                Display::print("> C to end. B to restart");
                if( buttonsJustPressed == B_C ){
                    end = true;
                }
                
                if( buttonsJustPressed == B_B){
                    step = 0;
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
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_B:
                    icons.play(hackIcons, HackIcons::bUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_C:
                    icons.play(hackIcons, HackIcons::cUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_UP:
                    icons.play(hackIcons, HackIcons::dUpUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_DOWN:
                    icons.play(hackIcons,  HackIcons::dDownUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_LEFT:
                    icons.play(hackIcons, HackIcons::dLeftUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_RIGHT:
                    icons.play(hackIcons,  HackIcons::dRightUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                default:
                    icons.play(hackIcons, HackIcons::empty);
                    icons.draw(8 + i * 16, 32);
            }
            
        }
        
        robo.draw(roboX, roboY);
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