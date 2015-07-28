# Introduction #

I started off looking for a bluetooth addition for my 2007 Toyota Matrix with no avail, but I came across 2 projects.

  * http://www.softservice.com.pl/corolla/avc/avclan.php
  * http://www.sigmaobjects.com/toyota/

Both of these projects indicated a method of enabling the CD changer port in the back of the head unit for audio input. Time to Get to work!

# Details #

## Hardware ##

The harware is very basic, I have an ATMega168 running on a 12Mhz ceramic resonator for the controller. The power is supplied via the 12V auxiliary rail on the CD changer connector, so I step it down with a 5V buck converter, then take it lower with a 3.3v LDO for voltage stability. We use the RS485 driver (SN75HVD11D) in place of an actual IEBUS driver, but the voltage levels match close enough that it doesn't matter. Actual drivers are very hard to come-by. For Bluetooth communication, I am using a Bluegiga WT32 with the iWrap4 firmware installed.

## AVC-Lan Driver ##

This was the most challenging part of the build. I realized very quickly that the examples provided could be optimized considerably for "threaded" run-time. I designed the receiving part of my system around the INT0 Interrupt, as it allowed me to jump in, service the pin change, determine the bit, add it in, acknowledge if needed, and jump out without skipping a beat. This means I don't have to stay "stuck" in an unserviceable state, and I will be able to catch the data in time with a much higher rate of success.

Send turned out to be a bit of a pain. I tried to use the CNT1 (16-bit) output compare to no avail as I had initially wanted. Frustrated, I resorted to sequential instructions based around the design by the SigmaObjects code. Instead of issuing a pin change, jumping out of the ISR and back in once the appropriate time came, I simply use the timer to count the time, and put the system in a while loop until that happens. Unfortunately, I think it was the ACKing that forced my hand, but I couldn't be sure as I didn't have access to an oscilloscope.

## "Task Manager" ##

This is the heart of the "OS". Tasks are posted to a queue, and handled via their period (or delay if one-shot) one at a time, until complete, moving to the next in queue. It is VERY basic, but does a good job at allowing relatively stable periodic task running.

## UART Driver ##

The UART driver was initially designed to be interrupt driven BOTH ways, but found that sending data via interrupt impaired the AVC-LAN driver too much (and couldn't be set to noblock for good reason :) ). So I left receive as an interrupt driven function, and put the UART sending off to it's own periodic task.

## Bluetooth ##
Bluetooth was made SUPER simple with the WT32 Bluegiga module, though I had to change the module communication rate to 9600 baud with my computer first. I do some simple sending and receiving of data, and I have hopes to get artist information up on display soon, along with Handsfree with mic up and running in the future.