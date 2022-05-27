# USB2PS2

The goal of this project is to provide people a cheap way to use USB keyboards/mice with <a href="https://templeos.org">TempleOS</a>/<a href="https://github.com/tinkeros/TinkerOS">TinkerOS</a>.  Many modern motherboards provide PS/2 emulation for USB keyboards only which leaves you with no mouse support unless you are willing to connect a PS/2 mouse.  Thankfully many motherboards are still available that have PS/2 ports.  You can use this project for adding mouse support via PS/2.  Keyboard support works also.  I use it to connect my TinkerOS machine to my USB KVM which provides both keyboard and mouse through one USB connection.

I built this project in my spare time while helping test out the <a href="https://github.com/dekuNukem/USB4VC">USB4VC</a> project which provides an excellent solution for people who want to use USB keyboards/mice/gamepads with operating systems that require ADB, PS/2, serial and/or gameport inputs.  If you don't want to build it yourself, you can just <a href="https://www.tindie.com/products/dekuNukem/usb4vc-usb-inputs-on-retro-computers/">buy the USB4VC</a> and it will "just work" for DOS/TempleOS/TinkerOS and many other operating systems.

If you are a DYI kind of person who likes to tinker with 3D printing/Arduinos/PIs/electronics, then this maybe useful to you.  This is a do at your own risk project!  I am not an electrical engineer and if you do this wrong you could fry your Arduino/Computer/PI or possibly even set your house on fire.  Only play with electronics if you feel like you are up to the task and accepting of the risks.

The PS/2 keyboard code is from ps2dev, the PS/2 mouse code is code adapted from USB4VC.  The PI SD card image is a USB4VC image which has been modified to communicate with an Arduino over serial instead of a USB4VC IBM PC protocol card using SPI.

### Suggested supplies:
- Arduino Uno (or other version if you're willing to adapt the code yourself)
- Raspberry PI (PI zero is the cheapest and will work fine for this project)
- Logic level converter (convert between PI 3.3V logic levels and Arduino 5V logic levels, possibly optional)
- 2x male PS/2 to male PS/2 cables (can use 1 instead if you cut and solder, see notes below, you might be able to make your own much cheaper)
- 2x female PS/2 ports to connect to the Arduino (optional, see notes below)
- USB cables/power supplies for powering the Arduino and PI
- Micro USB to USB-A female USB adapter (if using a PI zero) and USB hub (if you want to connect multiple devices)
- Jumper wires to connect the Arduino/PI/level converter

### Required Arudino libraries:
- ps2dev (I used version 0.0.3)

### Software Installation:
Installation simply requires extracting and flashing the <a href="https://github.com/tinkeros/USB2PS2/releases/download/v0.1.0-beta/usb4vc_arduino_mod.img.xz">usb4vc_arduino_mod.img.xz</a> PI image to a micro SD card.  It will auto resize the filesystem to fill the card on first boot and then reboot.  The username/password is pi/raspberry.  
The Arduino is flashed with the sketch that can be found in <a href="https://github.com/tinkeros/USB2PS2/tree/master/ArduinoUno/pi_to_ps2_keyboard_mouse/pi_to_ps2_keyboard_mouse">ArduinoUno/pi_to_ps2_keyboard_mouse/pi_to_ps2_keyboard_mouse</a>.  Be sure to modify the defined pin numbers at the top of the sketch to the values you desire to use.

### Software and circuit connections overview:
The PI captues keyboard/mouse events from attached USB devices and sends them to the Arduino over the UART.  You might be able to make this work by connecting just 2 wires between the Arduino and PI.  The PS/2 ports are connected to data/clock pins you choose on the Arduino and two ground pins.  More details on the connection setup can be found below.

### Supply notes:
- You can get away no female PS/2 ports and with just one male to male PS/2 cable if it is long enough and you cut it in half and connect/solder the wires directly to the Arduino.
- You might be able to get away with just connecting the PI TX pin to the Arduino RX pin or use a simple resister/transistor based circuit to convert the logic level instead of using a level converter.  
- There are Arduino Uno clones which provide additional headers you can use to solder components onto easily.  Here is an <a href="https://github.com/tinkeros/USB2PS2/raw/master/images/ArduinoClone.jpg">example</a> of one such board I bought cheap off AliExpress.

### Recommended connections connecting the Arduino / Level Converter / PI:
  
Arduino 5V   <-> Level Converter HV  
Arduino GND  <-> Level Converter GND  
Arduino RX   <-> Level Converter HV1  
Arduino TX   <-> Level Converter HV2 (optional)  
  
PI 3.3V <-> Level Converter LV  
PI GND  <-> Level Converter GND  
PI TX   <-> Level Converter LV1  
PI RX   <-> Level Converter LV2 (optional)  
  
<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/raspberrypi_arduino_serial_gpio.png" />
</p>

### Example using a simple 2 wire connection (works for me with an Arduino Uno clone and Rasperry PI Zero W):
  
Arduino GND  <-> PI GND  
Arduino RX   <-> PI TX 

<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/2_wire.jpg" />
</p>

### Wiring the PS/2 ports to the Arduino:
PS/2 is bitbanged so you may use any analog or digital pins (other than those used by the UART) for PS/2 communications.  For each PS/2 connection you want to use, you need to connect a clock, data, and ground pins.  Choose whatever pins you want to use for clock and data for each device and modify the defined values for PS2_MOUSE_CLOCK_PIN, PS2_MOUSE_DATA_PIN, PS2_KEYBOARD_CLOCK_PIN, and PS2_KEYBOARD_DATA_PIN as appropriate.  You do not need to connect the 5V PS/2 pin and should not connect it.

## Powering the Arduino and PI:
- I suggest powering the Arduino from the PC using the normal USB-B to USB-A cable you use to connect it to the PC and program it with.
- I suggest powering the PI with a micro USB to USB-A cable connected to the PC if using a PI Zero W or the official PI power supply.
- I do not connect the 5V pin on any of the PS/2 connectors or use them to draw power.  I highly advise against drawing power from the PS/2 ports.

## 3D printed PCI brackets for Arduino UNO/Raspberry PI Zero:
You can contain this project nicely inside your PC if you have two free PCI slots.  There are many 3D printable PCI brackets available for Raspberry PIs and Arduinos (just search thingiverse).  I've included the two STL files I used in the STL folder <a href="https://github.com/tinkeros/USB2PS2/raw/master/STL/pci_arduino.stl">Arduino UNO PCI bracket</a> and <a href="https://github.com/tinkeros/USB2PS2/raw/master/STL/single_zero_pi_pci.stl">PI zero PCI bracket</a>  Even if you do not own a 3D printer there are many online services you can use to get someone else to print them for you cheaply.  After printing you will want to cut/drill an extra slot into one of them for the PS/2 cables to go through.

### Example Arduino / PI mounted in brackets (note extra slot cut into the PI bracket for the PS/2 cables):

<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/PCI_brackets.jpg" />
</p>


### Installed setup with PS/2 cables coming out:

<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/Install_without_cables.jpg" />
</p>

### Installed setup with PS/2 cables, two USB cables powering Arduino and PI, and one micro USB to USB-A cable with bluetooth USB keyboard mouse combo reciever:

<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/Full_install.jpg" />
</p>


## Cheaply making PS/2 cables:
Real PS/2 cables are becoming expensive because they are rarely used and not mass produced anymore.  You are free to use them, however I would like to suggest an alternative that may be much cheaper and may work better for this particular project.  There is an abundance of cheap to obtain PS/2 to USB adapters and USB-A to A cables (just check eBay).  You can use these to make PS/2 male to male cables or for the case of this project you can simply cut the USB cord and connect/solder the proper wires to the proper Arduino pins.  If you are willing to solder wires and have some old USB-A cables of any type laying around, you can skip buying an A-A cable, buy the PS/2 male to USB-A female adapters only and just chop one end off your existing cables.

<p align="center">
  <img src="https://github.com/tinkeros/USB2PS2/raw/master/images/USB2PS2cable.jpg" />
</p>

### Acknowledgements:
<a href="https://github.com/dekuNukem/USB4VC">USB4VC project</a>
