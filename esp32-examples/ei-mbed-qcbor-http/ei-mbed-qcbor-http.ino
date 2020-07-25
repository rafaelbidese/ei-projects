/* Edge Impulse ingestion SDK
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// replace these accordingly
#define API_PATH  "http://ingestion.edgeimpulse.com/api/training/data"
#define API_KEY   "YOUR_API_KEY"
#define HMAC_KEY  "YOUR_HMAC_KEY"
#define SSID      "SSID"
#define PASSWORD  "PASSWORD"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qcbor.h"
#include "nonposix.h"
#include "sensor_aq.h"
#include "sensor_aq_mbedtls_hs256.h"

#include <WiFi.h>
#include <HTTPClient.h>

// avoid brownout - use with care
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"





void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

    Serial.begin(115200);
  
    // The sensor format supports signing the data, set up a signing context
    sensor_aq_signing_ctx_t signing_ctx;

    // We'll use HMAC SHA256 signatures, which can be created through Mbed TLS
    // If you use a different crypto library you can implement your own context
    sensor_aq_mbedtls_hs256_ctx_t hs_ctx;
    // Set up the context, the last argument is the HMAC key
    sensor_aq_init_mbedtls_hs256_context(&signing_ctx, &hs_ctx, HMAC_KEY);

    // Set up the sensor acquisition basic context
    sensor_aq_ctx ctx = {
        // We need a single buffer. The library does not require any dynamic allocation (but your TLS library might)
        { (unsigned char*)malloc(1024), 1024 },

        // Pass in the signing context
        &signing_ctx,

        // And pointers to fwrite and fseek - note that these are pluggable so you can work with them on
        // non-POSIX systems too. Just override the EI_SENSOR_AQ_STREAM macro to your custom file type.
        &ms_fwrite,
        &ms_fseek,
        // if you set the time function this will add 'iat' (issued at) field to the header with the current time
        // if you don't include it, this will be omitted
        NULL
    };

    // Payload header
    sensor_aq_payload_info payload = {
        // Unique device ID (optional), set this to e.g. MAC address or device EUI **if** your device has one
        "ac:87:a3:0a:2d:1b",
        // Device type (required), use the same device type for similar devices
        "DISCO-L475VG-IOT01A",
        // How often new data is sampled in ms. (100Hz = every 10 ms.)
        10,
        // The axes which you'll use. The units field needs to comply to SenML units (see https://www.iana.org/assignments/senml/senml.xhtml)
        { { "accX", "m/s2" }, { "accY", "m/s2" }, { "accZ", "m/s2" } }
    };

    // Place to write our data.
    memory_stream_t stream;
    stream.length = 0;
    stream.current_position = 0;

    // Initialize the context, this verifies that all requirements are present
    // it also writes the initial CBOR structure
    int res;
    res = sensor_aq_init(&ctx, &payload, &stream, false);
    if (res != AQ_OK) {
        Serial.printf("sensor_aq_init failed (%d)\n", res);
        while(1);
    }

    // Periodically call `sensor_aq_add_data` (every 10 ms. in this example) to append data
    float values[][3] = {
        { -9.81, 0.03, 1.21 },
        { -9.83, 0.04, 1.28 },
        { -9.12, 0.03, 1.23 },
        { -9.14, 0.01, 1.25 }
    };
    for (size_t ix = 0; ix < sizeof(values) / sizeof(values[0]); ix++) {
        res = sensor_aq_add_data(&ctx, values[ix], 3);
        if (res != AQ_OK) {
            Serial.printf("sensor_aq_add_data failed (%d)\n", res);
            while(1);
        }
    }

    // When you're done call sensor_aq_finish - this will calculate the finalized signature and close the CBOR file
    res = sensor_aq_finish(&ctx);
    if (res != AQ_OK) {
        Serial.printf("sensor_aq_finish failed (%d)\n", res);
        while(1);
    }

    // For convenience we'll print the encoded file. 
    // You can throw this directly in http://cbor.me to decode
    Serial.printf("Encoded file:\n");

    // Print the content of the stream here:
    for (size_t ix = 0; ix < stream.length ; ix++) {
        Serial.printf("%02x ", stream.buffer[ix]);
    }
    Serial.printf("\n");

    /*
     * 
     * Here the binary data stored in the stream object is
     * uploaded to the API
     * 
     */

    WiFi.begin(SSID, PASSWORD);
    
    Serial.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
        
    HTTPClient http;
    if(http.begin(API_PATH)){
      Serial.println("[HTTP] begin...");
    } else {
      Serial.println("[HTTP] failed...");
    }
    
    http.addHeader("content-type", "application/cbor");
    http.addHeader("x-api-key",  API_KEY);
    http.addHeader("x-file-name", "embeddedtest");
    http.addHeader("x-label", "test");
  
    int httpCode = http.POST(stream.buffer, stream.length);
    
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
        Serial.println(httpCode);
     } else {
        Serial.printf("[HTTP] failed, error: %d %s\n", httpCode, http.errorToString(httpCode).c_str());
     }
     http.end();
}

void loop(){}
