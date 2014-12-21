Robot-That-Anyone-Can-Program
=============================

This project is inspired by Rethink Robotics' Baxter - an industrial robot that can be taught how to perform a task just by moving its arms. The robot can be trained by anyone to run to-and-fro on a track(with linear path and turns), using an intuitive Android app, that is built using MIT AppInventor. To train the robot user drives the robot on the path, and the algorithm stores the path in the memory. 

The robot uses an Arduino Uno board as onboard computer, a bluetooth shield to connect with the Android phone and a chassis with two d.c. motors. The directions - forward, backward, turn right,and turn left are assigned numerical values. The software learns the track as a sequence of directions, stored in a 4 byte unsigned integer constructed by appending the integer with numerical values assigned to each direction. At the culmination of training the directions are stored in the EEPROM of Arduino, and are overwritten after re-training of robot.

This is my first robotics project, so the code is not tidy. I tried to break the code into different classes, but could not get it working that way. 
