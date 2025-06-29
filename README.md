# Arduino-based Out-of-Band Hardware monitor 
This was developed as my final project for an electronics programming class but I use it on a daily basis. I used the Arduino starter kit which includes an LCD screen that I used for the project. The hardware layout follows exactly Project 11 - Crystal Ball in the Arduino Starter Kit projects book.

The monitor connects from an Aruino Uno via USB to the PC and reads sensor data from LibreHardwareMonitor. It then sends the data over serial to the Arduino via a python script. I have a scheduled task setup to launch both LibreHardwareMonitor and the python script in the background on boot. That begins data transfer and output to the screen. 
