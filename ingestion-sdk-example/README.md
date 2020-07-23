# Edge Impulse C Ingestion SDK

The C Ingestion SDK is a portable header-only library written in C99 for data collection on embedded devices. It's designed to reliably store sampled data from sensors at a high frequency in very little memory. On top of this it allows cryptographic signing of the data when sampling is complete. Data can be stored on a POSIX file system, in memory, or on a raw block device.

## How to

I would like to add a little bit of what I have learned here running this example in a Windows machine with WSL.

### `ingestion-sdk-c` 

This is the exact example from the Edge Impulse's github

Open the WSL console.

If you dont have git, install git `sudo apt install git`, and:

`git clone --recurse-submodules https://github.com/edgeimpulse/ingestion-sdk-c.git`

You will also need `gcc` to run the Makefile, so `sudo apt install gcc` (you may have to `sudo apt update` first) and you can check your installation by `gcc -v`.

Then, you may `cd ingestion-sdk-c` and run `make`.

You can execute the file by `./ingestion-sdk-example`. This should create a file at `/test/` and output a cbor valid string in the terminal, that you can check at [`cbor.me`](http://cbor.me/).

You can run the tests using `node`. If you dont have it installed [there are instructions here](https://phoenixnap.com/kb/install-node-js-npm-on-windows).

Once you have it installed, install the cbor package `npm install cbor` and you can run the example:

`node decode.js`

You should see your `.cbor` file prited on your console.

### `ingestion-sdk-non-posix` 

Here I used the advices from the documentation to use the example on a device that does not have a file system. 

This was a step for me in understanding more how it works before testing it on an embedded platform.

The usage is the same as mentioned before and the only file that is different is the `/test/main.c` and the removed `.js` files.

You can still check your output string using [`cbor.me`](http://cbor.me/).


[Further Edge Impulse Documentation](https://docs.edgeimpulse.com/reference#c-sdk-usage-guide)