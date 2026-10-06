# Alarm-Clock

> **Implementation Note:** All peripheral drivers in this codebase were written entirely from scratch using ATMega2560 development board standard AVR libraries which are also available in my documentation. Direct register manipulation was performed using my own custom-built ATmega2560 memory and register map, [linked here](https://github.com/RaresCM/ATMega2560-Custom-Documentation).

## Overview
This is my own custom alarm clock built on the ATMega2560 development board platform in bare metal C. The build uses a 16/2 LCD, a DS1307 battery powered time module, an active speaker and analog components.

<img width="1637" height="928" alt="image" src="https://github.com/user-attachments/assets/e4510eca-3064-47ea-818e-a32cb5028170" />

## Key Features
This project was built to apply concepts from my EEE course as well as module integration and features:
* Custom 8 bit TWI protocol for sending and receiving data to the LCD and DS1307 clock module
* Custom cursor control for the LCD display to adjust the alarm using analog buttons
* Debounce logic and 2 analog potentiometers for contrast and brightness

## Notable issues and possible improvements

The current version has no physical way to adjust the start time and date, this must be done by hardcoding the start value directly into the C code and running the program once to initialise the clock module which from then on will keep the right time even if disconnected with minimal time drift over months.
