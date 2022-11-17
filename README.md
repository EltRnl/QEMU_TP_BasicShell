# QEMU TP - Bare Metal basic shell

## Goal

In this project, the goal was to implement functionalities to an existing qemu base :
- First was a simple shell, capable of handling incoming characters and commands. 
- Second was the handling of interruptions to prevent polling while waiting for inputs.

## How to run

Go into the **arm.boot** folder and run "make run", then have fun with the terminal. When you want to quit, press **Crtl+x** to quit, or **Crtl-a** then type "quit". If for whatever reason the last two methods don't work, go into another terminal, into the folder **arm.boot** and run "make kill" to kill the qemu process.

## Implemented functionalities

### The shell

The shell is limited to 200 characters and understands two commands :
- reset : clears the terminal (equivalent to Ctrl+l on linux)
- echo \[arg\] : prints the given argument on the terminal

The first version (polling one) was finished on commit "Part 1 Done - Version 1.0 Done" (5818f2ac96c371a727c40c299462aa91cd21e053).

### Interrupts

The Receive interrupts were implemented, meaning we don't poll (or spin) anymore waiting for a character.

## Branches

- master : contains all the kept functionalities
- week1_tests : branch used to implement the shell, later merged into master
- Part2 : branch used to implement the interrupts and the renewed shell, merged into master

#

More information regarding what has been done and what were the objectives in the folder **worklog**, for both parts on most commits.
