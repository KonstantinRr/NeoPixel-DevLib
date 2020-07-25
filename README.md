# NeoPixel-DevLib
A small library allowing to use Adafruit NeoPixel strips with all kind of cool effects.
This library aims at providing a simple interface in using multiple AdaFruit NeoPixel LED strips
with customized effects as well as using them for own effects. The library follows the design of
the Adafruit_NeoPixel library very closely.

## Setup
The library can be build along side most projects in the Arduino IDE. You just need to clone the repository and drop
the directory in the Arduino library directory. See this (link)[https://www.arduino.cc/en/guide/libraries]
for additional information on how to install libraries.

## Example
This example creates 4 different strips where two strips are inversed. The MultilineWrapper creates
an abstraction layer to index to each of the given NeopixelWrapper objects by an sequential index.
Therefore the 4 strips can be used as a single coherent strip.

```{c++}
NeopixelWrapper strips[] = {
  NeopixelWrapper(60, 11, NEO_GRB + NEO_KHZ800),
  NeopixelWrapper(57, 3, NEO_GRB + NEO_KHZ800, true),
  NeopixelWrapper(60, 6, NEO_GRB + NEO_KHZ800),
  NeopixelWrapper(57, 10, NEO_GRB + NEO_KHZ800, true),
};

MultilineWrapper strip(strips, 4);
```

## API

### NeopixelWrapper
A tiny wrapper that encloses an Adafruit_NeoPixel object.
It defines an aditional argument that determines whether the
strip is inversed or in regular order. The wrapper supports
the same strip functions as the Adafruit_NeoPixel object.
All indices that are given to this wrapper are adapted to the
reversed indices if the strip is inversed.

The wrapper takes up one additional byte of storage to
determine the strip order. The wrapper subclasses the
Adafruit_NeoPixel object which makes it a safe operation
to cast the wrapper to a Adafruit_NeoPixel. It inherits
the Adafruit_NeoPixel as protected member to hide unwanted
functionality.

### MultilineWrapper
This class encloses and manages multiple NeopixelWrapper objects.
It is used to chain multiple strips together to form a combined
strip. The strips may be inverted in which case the NeopixelWrapper
inverse flag should be set to true. All strips need to have the same
internal pixel format (RGB, BGR, etc.), but may have different length.

This class creates an additional layer of abstraction that maps each
virtual index to a hardware pointer that stores the first byte of each
pixel. This takes another 2 bytes per pixel on most micro controllers.
It allows the fast indexing of each hardware position by the virtual index.
