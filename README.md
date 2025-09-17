# cms-lantern-controller
Embedded firmware for a CMS Lantern Controller based on STM32F0, supporting PWM driven lantern outputs with Modbus RTU control.
    
Features:

⦁	STM32F0 firmware written in C with STM32CubeIDE + HAL drivers
⦁	Modbus RTU slave implementation at 19200 8N2 for remote control   
⦁	Configurable modes: ALL OFF, ALL ON, ALL FLASH (in phase), SIDE/SIDE (alternating)   
⦁	Brightness control (0–255) mapped to PWM duty cycle   
⦁	Power on blink sequence for status indication (2–3 blinks)   
⦁	ADC based current monitoring for each lantern channel   
⦁	Robust exception handling for invalid Modbus function codes and register writes   Modular code structure ("pwm.c", "app.c", "modbus.c", eg.) for clarity and maintainability

  
This repository is intended as a reference and testing implementation for Brightway Electronics' lantern controller prototypes. It demonstrates real time control, communication and diagnostics over Modbus and can be adapted for other embedded lighting systems.
