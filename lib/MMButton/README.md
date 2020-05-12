# ToneSfx library for Arduino

## Description

This library uses [VirtualDelay](http://www.avdweb.nl/arduino/libraries/virtualdelay.html) for handling non-blocking behavior.

## Library Usage   




## Example  
```$xslt
----------+ +-+ +-+                                       +-------------
          | | | | |                                       |
          | | | | |                                   +-+ |
          +-+ | | |                                   | | |
              | | |                                   | | |
              | | |                                   | | |
              | | |                                   | | |
              | | |                                   | | |
              +-+ +-----------------------------------+ +-+

          A          B          C    D    D    D    D                            


















```



### Instantiation/Creation
`MMButton first_button(private_pin);`


* `private_pin` - private_pin the button is connected to, assumes PULL_UP, Normally Open ()

### Methods
* `tick()` - ticker, has to be called repeatadly at a reasonable frequency


### Example
Example description...
```
#include <Arduino.h>
#include <MMButton.h>

MMButton aButton (10);

void setup() {

}

void loop() {
    aButton.tick();
}

```

