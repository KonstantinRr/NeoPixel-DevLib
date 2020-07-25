/// MIT License
/// 
/// Copyright (c) 2020 Konstantin Rolf
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#ifndef NEOPIXEL_WRAPPER_H
#define NEOPIXEL_WRAPPER_H

#ifndef DONT_INCLUDE_ADA
#include <Adafruit_NeoPixel.h>
#endif

//// ---- Defines which effects to include---- ////
// Only including the needed effects may shrink the sketch size
#ifndef INCLUDE_BLINKER
#   define INCLUDE_BLINKER 1
#endif

#ifndef INCLUDE_RUNNER
#   define INCLUDE_RUNNER 1
#endif

#ifndef INCLUDE_COLOR_CHANGER
#   define INCLUDE_COLOR_CHANGER 1
#endif

/// class NeopixelWrapper
/// A tiny wrapper that encloses an Adafruit_NeoPixel object.
/// It defines an aditional argument that determines whether the
/// strip is inversed or in regular order. The wrapper supports
/// the same strip functions as the Adafruit_NeoPixel object.
/// All indices that are given to this wrapper are adapted to the
/// reversed indices if the strip is inversed.
///
/// The wrapper takes up one additional byte of storage to
/// determine the strip order. The wrapper subclasses the
/// Adafruit_NeoPixel object which makes it a safe operation
/// to cast the wrapper to a Adafruit_NeoPixel. It inherits
/// the Adafruit_NeoPixel as protected to 
class NeopixelWrapper : protected Adafruit_NeoPixel
{
protected:
    /// Whether the strip is inversed.
    bool inverse;

public:
    /// Creates a new neopixel wrapper that takes the same arguments as an
    /// Adafruit_NeoPixel object. See the following reference for more
    /// information. It offers an additional argument that defines whether
    /// the strip is inversed or not. The default value is false.
    ///
    // Adafruit_NeoPixel objects are declared with the following arguments.
    // Argument 1 = Number of pixels in NeoPixel strip
    // Argument 2 = Arduino pin number (most are valid)
    // Argument 3 = Pixel type flags, add together as needed:
    //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
    //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
    //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
    //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
    //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
    NeopixelWrapper(
        uint16_t pixels, uint16_t pin,
        neoPixelType flags, bool inverse=false);
    
    /// (1) Returns whether this strip is inversed
    /// (2) Sets whether this strip is inversed
    inline bool isInversed() { return inverse; }
    inline bool setInversed(bool v) { inverse = v; }

    /// Returns the true memory index determined by the given virtual index.
    /// The strip index is determined by the following inference:
    /// (inverse ? length - 1 - index : index).
    uint16_t getIndex(uint16_t vindex);
    /// Returns the true memory position determnied by the given virtual index.
    /// The returned pointer gives the position of the first byte representing
    /// this pixel. Use rOffset, gOffset, bOffset and wOffset to write the
    /// color parts to the correct index.
    uint8_t* getPointer(uint16_t vindex); 

    /// (1) Sets the color of a single pixel determined by the virtual index.
    /// (2) Sets the color of a single pixel determined by the virtual index.
    /// (3) Sets the color of a single pixel determined by the virtual index.
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void setPixelColor(uint16_t n, uint32_t c);

    /// (1) Fills the whole strip with the given color. This functions does not
    /// follow special behaviour for an inversed strip.
    /// (2) Fills the strip from a beginning point to the end.
    /// (3) Fills the interval in the given interval
    void fill(int32_t color);
    void fill(int32_t color, int16_t start);
    void fill(int32_t color, int16_t start, uint16_t count);

    /// (1) Returns the internal offset of the red value
    /// (2) Returns the internal offset of the green value
    /// (3) Returns the internal offset of the blue value
    /// (4) Returns the internal offset of the white value
    inline uint8_t getROffset() { return rOffset; }
    inline uint8_t getGOffset() { return gOffset; }
    inline uint8_t getBOffset() { return bOffset; }
    inline uint8_t getWOffset() { return wOffset; }

    /// (1) Returns whether this strip is an 800KHz strip
    /// (2) returns whether this strip is an RGB or RGBW strip
    inline bool is800KHzStrip() { return is800KHz; }
    inline bool isRGB() { return wOffset == rOffset; }

    //// ---- Additional wrapper functions ---- ////

    /// Returns the number of pixels in this strip. This function
    /// wraps the Adafruit_NeoPixel::numPixels function.
    inline int8_t numPixels() { return Adafruit_NeoPixel::numPixels(); }

    /// (begin) wraps Adafruit_NeoPixel::begin function
    /// (show) wraps Adafruit_NeoPixel::show function
    /// (setPin) wraps Adafruit_NeoPixel::setPin function
    /// (clear) wraps the Adafruit_NeoPixel::clear function
    /// (updateLength) wraps the Adafruit_NeoPixel::updateLength function
    /// (updateType) wraps the Adafruit_NeoPixel::updateType function
    inline void begin(void) { Adafruit_NeoPixel::begin(); }
    inline void show(void) { Adafruit_NeoPixel::show(); }
    inline void setPin(uint16_t p) { Adafruit_NeoPixel::setPin(p); }
    inline void clear(void) { Adafruit_NeoPixel::clear(); }
    inline void updateLength(uint16_t n) { Adafruit_NeoPixel::updateLength(n); }
    inline void updateType(neoPixelType t) { Adafruit_NeoPixel::updateType(t); }
};

/// class MultilineWrapper
/// This class encloses and manages multiple NeopixelWrapper objects.
/// It is used to chain multiple strips together to form a combined
/// strip. The strips may be inverted in which case the NeopixelWrapper
/// inverse flag should be set to true. All strips need to have the same
/// internal pixel format (RGB, BGR, etc.), but may have different length.
///
/// This class creates an additional layer of abstraction that maps each
/// virtual index to a hardware pointer that stores the first byte of each
/// pixel. This takes another 2 bytes per pixel on most micro controllers.
/// It allows the fast indexing of each hardware position by the virtual index.
class MultilineWrapper
{
protected:
    /// The wrapper objects that are managed by this object
    /// as well as the object count.
    NeopixelWrapper *wrappers;
    uint8_t wrapperCount;

    /// Counts the total amount of pixels managed by this object
    uint16_t pixelCount;

    /// Stores a pointer for each index that gives the pixel's
    /// hardware address.
    uint8_t **indexMap;

    /// Stores the internal pixel offsets for the different components.
    /// These are inherited from the NeopixelWrapper objects. 
    uint8_t rOffset; // Internal red offset
    uint8_t gOffset; // Internal green offset
    uint8_t bOffset; // Internal blue offset
    uint8_t wOffset; // Internal white offset

public:
    /// Creates a new MultilineWrapper object that manages
    /// the given NeopixelWrapper objects.
    MultilineWrapper(NeopixelWrapper *wrappers, uint8_t wrapperCount);

    /// Destroys this object and frees the allocated resources.
    ~MultilineWrapper();

    /// Assigns the wrappers to this MultilineWrapper.
    /// This function creates a new abstraction layer and destroys
    /// the old one.
    void setWrappers(NeopixelWrapper *w, uint8_t num);

    /// (1) Returns the strips that are managed by this object.
    /// (2) Returns the amount of strips managed by this object.
    inline NeopixelWrapper* getWrappers() { return wrappers; }
    inline uint8_t numWrappers() { return wrapperCount; }

    /// (1) Returns whether all strips are RGB strips.
    /// (2) Returns the number of combined strips in all pixels.
    inline bool isRGB() { return rOffset == wOffset; }
    inline uint16_t numPixels() { return pixelCount; }

    /// (1) Sets the color of a single pixel determined by the virtual index.
    /// (2) Sets the color of a single pixel determined by the virtual index.
    /// (3) Sets the color of a single pixel determined by the virtual index.
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void setPixelColor(uint16_t n, uint32_t c);

    /// (1) Fills the whole strip with the given color. This functions does not
    /// follow special behaviour for an inversed strip.
    /// (2) Fills the strip from a beginning point to the end.
    /// (3) Fills the interval in the given interval
    void fill(int32_t color);
    void fill(int32_t color, int16_t start);
    void fill(int32_t color, int16_t start, uint16_t count);

    /// (1) Calls the show method of all underlying wrapper objects.
    /// (2) Calls the begin method of all underlying wrapper objects.
    /// (3) Calls the clear method of all underlying wrapper objects.
    void show();
    void begin();
    void clear();
};

//// ---- Effects ---- ////
// Each effect contains a wrapper variable that must be set.
// It determines the strip that is manipulated by this effect.

#if INCLUDE_BLINKER

/// A simple blinking effect that changes between two colours.
/// The colours can be adapted by changing the colorOn and colorOff
/// member settings.
struct Blinker
{
    MultilineWrapper *wrapper;
    
    // counts the current loop state
    uint8_t state = 0;

    int32_t colorOn = Adafruit_NeoPixel::Color(255, 255, 255);
    int32_t colorOff = Adafruit_NeoPixel::Color(0, 0, 0);

    void update();
};

#endif

#if INCLUDE_RUNNER

/// A simple line of pixels that runs around the strip. It starts again
/// at the beginning if it reaches the end of the strip. The direction
/// parameter can be changed to determine the runner's direction and
/// speed. The length parameter gives the runner's size. The whole
/// runner is painted with a solid color.
struct Runner
{
    MultilineWrapper *wrapper;

    // counts the current loop state
    int16_t state = 0;

    int32_t color = Adafruit_NeoPixel::Color(255, 0, 0);

    int8_t direction = 1;
    uint8_t length = 1;

    void update();
};

#endif

#if INCLUDE_COLOR_CHANGER

/// This effect implements a smooth colour changer between the two
/// given colors. It starts at the first color and slowly changes 
/// to the second one. It changes back to the first color afterwards
/// giving a smooth transitions.
struct ColorChanger
{
    MultilineWrapper *wrapper;

    int32_t colorStart = Adafruit_NeoPixel::Color(255, 255, 255);
    int32_t colorEnd = Adafruit_NeoPixel::Color(0, 0, 0);

    void update();
};

#endif

#endif
