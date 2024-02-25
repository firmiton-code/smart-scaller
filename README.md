# NutriScale 
# smart-scaller

**Nutrition Scale designed from scratch.** 

NutriScale is primarily designed to measure five types of food nutrition parameters, such as Calories, Fat, Protein, Carbohydrat, and Carbing.

![MagicEraser_240206_223326](https://github.com/firmiton-code/smart-scaller/assets/75991391/b8565aea-b6f1-466f-b456-17622f208d94)
![photo_2024-01-02_11-51-14](https://github.com/firmiton-code/smart-scaller/assets/75991391/33677d6c-4593-41e9-aab3-265d1707c080)

# HADES Flight Control System
**Complete flight control system designed from scratch.** The flight control system is primarily designed for fixed-wing, autonomous drones to enable them to perform tasks autonomously, such as take-off, landing, waypoint navigation, payload delivery, etc.

Every part of this system is designed from first principles! This includes the hardware, firmware (using FreeRTOS), Kalman filtering for state estimation, control algorithms, base station, communication protocol, flight simulator, signal processing, and much, much more.

*Feel free to send me constructive criticism or things you would like me to implement, or even if you would like to help me develop this system!*

<img src="http://philsal.co.uk/wp-content/uploads/2019/12/Hades-Assembly-Front.jpg"
     alt="Markdown Monster icon"
     style="display: block;
  margin-left: auto;
  margin-right: auto;
  width: 50%;"/>
  
Intro video: https://www.youtube.com/watch?v=rLDqQ2L_mUQ
       
---

**Broadly, this repository consists of:**
* Hardware designs (developed using KiCAD).
* Low-level drivers and firmware (FreeRTOS, for the STM32 platform of microcontrollers).
* Higher-level firmware such as communication protocols, control and navigation algorithms, etc.
* Kalman filter designs for state estimation.
* Debugging tools.
* Flight simulator (both 6-DOF and 3-DOF) to test algorithms.


---


**Current repository folder structure:**
* *Calibration:* Raw sensor data useful for checking calibration algorithms (e.g. accelerometer bias, scale factor, etc.).
* *Communication:* UAVLink data protocol for reliably packing and transferring data and commands between the aircraft and a further device (e.g. base station). This could be via telemetry or direct serial links.
* *Control:* Algorithms for dynamic control of the aircraft (e.g. PI, state feedback, etc.).
* *Data:* Collection of various datasets that may be useful for offline testing (e.g. raw IMU data).
* *Debug Software:* Tools for interfacing with the hardware (e.g. to read navigation data).
* *Documentation:* Preliminary documents to detail aspects of the flight  control system.
* *Filtering:* FIR and IIR filter implementations, as well as FIR filter design tool.
* *Firmware:* The core flight control firmware running on the STM32 microcontrollers.
* *Flight Simulator:* 6-DOF and 3-DOF fixed-wing flight simulators developed in Unity. Very useful for testing control logic and algorithms. Requires dynamic model of aircraft.
* *GPS:* NMEA sentence parser, as well as raw GPS data for testing.
* *Ground Control:* Basestations written in C# to send commands to aircraft and receive data via telemetry. Includes moving map, live plots, etc.
* *Hardware:* HADES flight control board design files (schematic, layout, assembly). Hardware consists of sensors, microcontrollers, power electronics, etc. This custom hardware is where the firmware runs on.
* *State Estimation*: Kalman filters (various types: quaternion, Euler, different states estimated) designed in Matlab and exported to C for estimation of states required for feedback control (e.g. attitude, position, etc.).
* *Tests:* Miscallaneous tests go here. For instance, device driver testing, communication protocol tests, etc.

**Note:** Most subfolders include an additional README that further describes its contents.

---

**Disclaimer:** This flight control system is work in progress. Use the files, hardware, and software given in this repository **at your own risk** - especially when conducting live flight tests.

BSD 3-Clause License - Copyright (c) 2019, Philip Salmony - All rights reserved.
