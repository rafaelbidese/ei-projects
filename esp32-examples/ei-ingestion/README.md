# Edge Impulse C Ingestion SDK

The C Ingestion SDK is a portable header-only library written in C99 for data collection on embedded devices. It's designed to reliably store sampled data from sensors at a high frequency in very little memory. On top of this it allows cryptographic signing of the data when sampling is complete. Data can be stored on a POSIX file system, in memory, or on a raw block device.

# ESP32 setup

If you don't have the Arduino environment setup to use the ESP32 boards, I suggest following [this tutorial](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

* Depending on your module, you will need a FTDI to program your ESP32 modules. I am bypassing a NodeMCU (ESP8266) as a serial converter. 
* Also, if you get brownout errors on your device, it's usually due to the power source not being able to provide enough current to the board (especially if you are using the camera). Use a better cable or an external power source. 
* Moreover, I would suggest setting your partition scheme to the `Huge app` option so that you don't have any problem. 
* Finally, to be able to flash your device you have to `SHORT` the `IO0` pin and the `GND` and reset the board. If you wanna run the code, disconnect the two pins and reset the board again.


## How to

You will have to copy the two folders `/edgeimpulse` and `/qcbor` to your Arduino libraries folder. 

On a Windows machine it is typically at:

`{username}/Documents/Arduino/libraries/`. 

The `qcbor` files were taken directly from the [original project](https://github.com/laurencelundblade/QCBOR/tree/cd721590af28bd80d99fadf0179ece3cd5a5bda0) branch that the ingestion-sdk-c uses.

The `/edgeimpulse/sensor_aq.h` and `/edgeimpulse/sensor_aq_mbedtls_hs256.h` are directly from the [ingestion-sdk-c](https://github.com/edgeimpulse/ingestion-sdk-c). The `/edgeimpulse/nonposix.h` is based on the example available on the sdk documentation.

**You don't have to import or install the `mbedtls` library since it is already part of the ESP32 sdk installation.**


[Further Edge Impulse Documentation](https://docs.edgeimpulse.com/reference#c-sdk-usage-guide)