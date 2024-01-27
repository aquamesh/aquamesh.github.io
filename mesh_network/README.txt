Basic Proof-of-Concept Instructions For Dummies
1. Run setNodeId.ino with respective node ID set in code, starting with "1"
	- Modifies EEPROM on-board memory to store Node Id before usage
	- Do this directly from computer to ensure that it works via Serial Monitor
2. Upload aquamesh.ino onto each board
3. Using USB power, battery connected into GND and Vin pins, or plugged into a
   a computer. 

Baud rate should be 57600 in Serial Monitor. Note that you may have to close
the serial monitor each time you upload an Arduino file.

Battery recharging is complicated. Still looking into a reliable method, beyond
connecting directly into a power supply. USB devices exist for this purpose,
but we ran out of money a long time ago. 


In the future, we should try to have:
 - Batch production/code upload
 - better mesh algo
 
 
If you have any questions, ask Scott @scottweb6@gmail.com