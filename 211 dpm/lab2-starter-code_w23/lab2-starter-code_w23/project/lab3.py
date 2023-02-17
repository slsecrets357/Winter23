#!/usr/bin/env python3

"""
Module to play sounds when the touch sensor is pressed.
This file must be run on the robot.
"""
 
from utils import sound
from utils.brick import TouchSensor, wait_ready_sensors
import time

sounds = []
NOSOUND = sound.Sound(duration=0.3, pitch="A4", volume=0)
SOUND1 = sound.Sound(duration=0.3, pitch="A4", volume=60)
SOUND2 = sound.Sound(duration=0.3, pitch="B4", volume=60)
SOUND3 = sound.Sound(duration=0.3, pitch="C5", volume=60)
SOUND4 = sound.Sound(duration=0.3, pitch="D5", volume=60)
sounds.append(SOUND1)
sounds.append(SOUND2)
sounds.append(SOUND3)
sounds.append(SOUND4)
touch1 = TouchSensor(1)
touch2 = TouchSensor(2)
touch3 = TouchSensor(3)
wasPressed = [0,0,0]
pressed = [0,0,0]
wait_ready_sensors() # Note: Touch sensors actually have no initialization time


def action(state):
    if state == 3 or state >= 5:
        "Play a single note."
        print("playing a single note")
        state = 4 if state==3 else state
        sounds[state-4].play().wait_done()
    elif state == 0:
        print("no sound")
        NOSOUND.play()
    else: #emergency stop
        print("emergency exit")
        exit()

def play():
    initialState = True
    wasPressed = [0,0,0]
    while True:
        #check buttons pressed
        pressed[0] = touch1.is_pressed()
        pressed[1] = touch2.is_pressed()
        pressed[2] = touch3.is_pressed()
        if not initialState:
            #play drum
            pass
        for i in range(3):
            #update wasPressed
            if pressed[i] and not wasPressed[i]:
                wasPressed[i] = 1
                initialState = False
        #if a button is released
        if wasPressed[0]>pressed[0] or wasPressed[1]>pressed[1] or wasPressed[2]>pressed[2]:
            #determine state based on wasPressed
            #touch1 is MSB, touch3 is LSB
            state = wasPressed[0]*4+wasPressed[1]*2+wasPressed[2]
            print(f"a button is released. performing action {state}")
            #perform action based on state
            action(state)
            #sleep to allow for all buttons to be released
            print("sleeping..")
            time.sleep(0.3)
            #reset wasPressed
            wasPressed = [0,0,0]

if __name__=='__main__':
    play()
