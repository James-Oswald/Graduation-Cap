# Graduation Cap
The janky arduino code for my mortarboard hat at the SUNY Albany 2021 Comencement Ceremony. 

## Features
- psudo-multitasking, handles multiple tasks via functions with lots of static vars and calling for current time alot. 
- Persistence of vision dispaly implementation for the 7 segment displays and binary counter. Only 1 led is on at a time on each 7 segment and only 1 for the whole binary counter, they switch so fast that to the human eye they appear to all be on at the same time. This allows for only 1 resistor to be used for each display as well as for the binary counter.
- Prime number computer: computes all prime numbers under 10000 and displays them on the 7 segment display.
- Binary counter: counts from 0 to 255 in binary and then resets.

## Pictures 
Picture of the hat working on my head durring the 2021 commencement ceremony.
![./onHead.png](./onHead.png)

Picture of the hat running on the table under better lighting conditions.
![./onTable.png](./onHead.png)