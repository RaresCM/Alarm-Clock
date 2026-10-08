# Alarm-Clock

> **Implementation Note:** Drivers for TWI and the LCD display were written entirely from scratch by me using ATmega 2560 standard AVR libraries which are also available in my documentation. Direct register manipulation was performed using my own ATmega 2560 memory register map, [linked here](https://github.com/RaresCM/ATMega2560-Custom-Documentation).

## Overview
My custom alarm clock built on a ATmega 2560 development board platform in bare metal C. The build uses a 16/2 LCD, a DS1307 battery powered time module, an active speaker and analog components.

<img width="1637" height="928" alt="image" src="https://github.com/user-attachments/assets/e4510eca-3064-47ea-818e-a32cb5028170" />

## Key Features
This project was built to apply concepts from my EEE course as well as module integration and features:
* Custom 8 bit TWI protocol for sending and receiving data to the LCD and DS1307 clock module
* Custom cursor control for the LCD display to adjust the alarm using analog buttons
* De-bounce logic and 2 analog potentiometers for contrast and brightness
* Input for setting an alarm and silencing it physically

## Issue and possible improvement

This version has no physical way to set the time and date, this must be done once in the C code by hard coding it for the DS1307 module. Afterwards the battery powered clock module will keep that time even if the board has no power. 

## Repository Structure
- /main.c - The main code for the clock to be uploaded to the chip
- /platformio.ini - PlatformIO settings to be used with VS code and PlatformIO extension
- /designlog.md - My log of findings, learning and obstacles encountered during the design of the alarm clock
