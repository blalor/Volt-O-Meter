"Volt-O-Meter" Driver
=====================

This is a circuit and associated firmware for driving a 40V analog meter without needing to modify the meter.  It can operate from batteries or a wall wart, and accepts an XBee radio to allow for remote updating of the output voltage.

## Overview

In April of 2011 I bought an antique volt meter (which my wife christened the "Volt-O-Meter") at an antique store in Ithaca, NY.  It appears to be a piece of telephone switchboard test equipment.  I haven't figured out what it was actually used for, but it's big, heavy, and has lots of switches.  It's therefore the perfect living room end table for the aspiring mad scientist.

The meter itself is in very good condition and has a seal on the back to prevent tampering.  Ironically, the front faceplate is completely removable, allowing access to the innards of the gauge.  It reads from 0 to 40 volts.  Analog meters of this sort are current driven devices, and I could have pretty easily modified the meter so that I could drive it to full deflection with 3.3 volts from the output pin of a microcontroller, but on the off chance that this thing might actually be worth something someday (it's already 80 years old, I figure), I wanted to modify it as little as possible.

I already have a system in place to monitor the power usage (well, currently just amperage draw) of my house.  I wanted to make this volt meter reflect the relative real-time power usage, both as a show of my über-geekery, and as a conversation piece.  My home automation server receives the power usage data and sends messages to this driver to set the meter needle to the desired position.  I have not yet published that code, but there is a sample Python script (`test/support/set_meter.py`) that shows how to create and send a properly formatted message to the device.

## Design

### Microcontroller

The microcontroller used here is an Atmel ATmega168 running on the 8MHz internal oscillator.  Since there is no provision for an external crystal/oscillator, make sure your fuses are programmed correctly before soldering the chip in!

### Serial Protocol

I'm a bit paranoid when it comes to transmitting serial data.  I use a very simple protocol that contains a 2-byte start-of-transmission marker (`0xff 0x55`), the packet length, the actual packet data, and a 1-byte checksum of the contents of the packet.  See the code and sample Python script for implementation details.

### Boost Converter

I used [Ladyada's DIY boost converter calculator](http://www.ladyada.net/library/diyboostcalc.html) to design the voltage booster. I'm self-taught in electronics and have largely bypassed learning the physics that make understanding boost converters possible.  As such, this one really doesn't work as designed.  Perhaps because I'm using some under-sized components in the version that I actually built (parts in the circuit/board design are not exactly what I actually used, and have a higher likelihood of working), I'm only able to actually generate about 34 volts.  Close enough for government work…  

### Controllable parameters

#### Voltage

The boost converter's transistor (T1) is driven from pin PD6.  The PWM frequency is as fast as the microcontroller will go, or 31,250Hz.  The duty cycle controls the voltage level; 89% gives the maximum voltage (derived experimentally).  The boost converter operates in open-loop mode, but the board does have a place for a pair of resistors, so that a voltage divider can be implemented.  The output of the voltage divider goes to one of the analog inputs on the µC.  Once the output voltage is set, it really shouldn't change, but if you want to drive something with a variable load, you may need to adjust the PWM duty cycle in response to the actual output voltage.

The boost PWM duty cycle can be set by sending a 2-byte packet, in the form `B<duty cycle>`, where the duty cycle goes from 0 to 255.

#### Meter drive

The antique meter is very slow to respond and seems to have a fair bit of capacitance.  As such, using a fast PWM frequency caused it to pretty much just show the full boosted voltage all the time.  I needed to use a much slower PWM frequency to allow the meter to correctly display the desired voltage.  

The effective voltage displayed on the volt meter is controlled by varying the duty cycle of transistor T2, and is set by sending a 2-byte packet in the form `M<duty cycle>` where, again, the duty cycle ranges from 0 to 255.  The duty cycle should set the output voltage linearly from 0 to the boost converter's output voltage.

#### LED

I originally intended to illuminate the face of the volt meter with a few LEDs, and I wanted to be able to set the intensity, so that the look could be fine-tuned.  Turns out I couldn't do that without modifying the meter, so I've decided to hold off, for now.  The board and the firmware support this output.  Set the LED intensity by sending a 2-byte packet in the form `L<duty cycle>`.  BYO current-limiting resistor!

## Connecting It Up

The individual connections on the board are pretty well labelled, but the polarities are not.  

### Meter

Connect the positive side of the meter to the top terminal of the block labelled `METER` (closest to capacitor C2).  The left side of that terminal block is connected to the collector of T2 and is connected to ground according to the meter drive PWM signal.

### LEDs

The polarity of the `LEDS` terminal block is the same as that for the meter.  Top is positive, bottom is ground (switched via T3).

### Power Switch

An external power switch is connected via the `SWITCH` terminal block.  If you want it to be powered all the time, short that connection.

### Power Sources

There are two provisions for powering the device.  The main voltage is 3.3 volts; any input voltage that is acceptable to the [MCP1700-3302E](http://www.mouser.com/ProductDetail/Microchip/MCP1700-3302E-TO/?qs=h7tZ5KkzNMMPEB66r2rMQw%3d%3d) will work.  If a center-positive external power supply is connected, the `BATTERY` terminal block will be bypassed.  If no external power supply is connected, the `BATTERY` terminal block's negative (bottom) terminal is connected to ground.  If you wish to use a battery pack, the top terminal is positive and the bottom is ground.

## Compiling

In the `main` directory, modify `Makefile`, setting `PROGRAMMER` to the ISP programmer and port you have.  Ensure `avr-gcc` and associated tools are on your `PATH`, then type

    make fuse

to program the fuses, followed by

    make main_flash

to install the program.
