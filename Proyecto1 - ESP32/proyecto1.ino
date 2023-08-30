// Adafruit IO Digital Input Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-digital-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.

#include "config.h"

/************************ Example Starts Here *******************************/

// digital pin 5
#define PUERTA_INPUT 23
#define FOCO_INPUT 22

// button state
bool current_puerta = false;
bool last_puerta = false;

bool current_foco = false;
bool last_foco = false;

int FOCO = 0;
int PUERTA = 0;

// set up the 'digital' feed
AdafruitIO_Feed *foco = io.feed("foco");
AdafruitIO_Feed *puerta = io.feed("puerta");

void setup() {

  // set button pin as an input
  pinMode(PUERTA_INPUT, INPUT);
  pinMode(FOCO_INPUT, INPUT);

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  io.run();

  PUERTA = digitalRead(PUERTA_INPUT); 
  FOCO = digitalRead(FOCO_INPUT);

  Serial.print("Estado de puerta -> ");
  Serial.println(PUERTA);
  puerta->save(PUERTA);

  Serial.print("Estado de foco -> ");
  Serial.println(FOCO);
  foco->save(FOCO);
  Serial.println(" ");

  delay(3000);
}

