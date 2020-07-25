# Edge Impulse C Ingestion SDK

The C Ingestion SDK is a portable header-only library written in C99 for data collection on embedded devices. It's designed to reliably store sampled data from sensors at a high frequency in very little memory. On top of this it allows cryptographic signing of the data when sampling is complete. Data can be stored on a POSIX file system, in memory, or on a raw block device.

## Examples

Here I used the advices from the documentation to use the example on a device that does not have a file system. 

I have tested the code on an ESP32-CAM using the Arduino environment and further I intend to use to this setup to capture image and post it to the ingestion API.

So far, I can:

* Encode regular the standard message to CBOR
* Post standard data to the Ingestion API

Next steps are:

* Encode image and send it to the Ingestion API

[Further Edge Impulse Documentation](https://docs.edgeimpulse.com/reference#c-sdk-usage-guide)