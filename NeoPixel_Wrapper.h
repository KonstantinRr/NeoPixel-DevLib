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

/// A tiny wrapper that encloses an Adafruit_NeoPixel object.
/// It defines an aditional argument that determines whether the
/// strip is inversed or in regular order. All calls to this wrapper
/// are adapted to the inversed indices.
class NeopixelWrapper : protected Adafruit_NeoPixel {
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
        int8_t pixels, int8_t pin,
        neoPixelType flags, bool inverse=false);
    
    /// (1) Returns whether this strip is inversed
    /// (2) Sets whether this strip is inversed
    inline bool isInversed() { return inverse; }
    inline bool setInversed(bool v) { inverse = v; }

    /// Returns the true memory index determined by the given virtual index.
    /// The index can be determined by the following inference:
    /// (inverse ? length - 1 - index : index).
    uint16_t getIndex(uint16_t vindex);
    /// Returns a pointer that gives the address of the beginning of the pixel.
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
    void fill(int32_t color, uint16_t start);
    void fill(int32_t color, uint16_t start, uint16_t count);

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

/// A wrapper class that encloses and manages multiple NeopixelWrapper
/// objects. It creates an array that maps each index to a pointer
/// giving the real address.
class MultilineWrapper {
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

    /// Stores the internal pixel offsets
    uint8_t rOffset;
    uint8_t gOffset;
    uint8_t bOffset;
    uint8_t wOffset;

public:
    /// Creates a new MultilineWrapper object that manages
    /// the given NeopixelWrapper objects. 
    MultilineWrapper(NeopixelWrapper *wrappers, uint8_t wrapperCount);

    /// Destroys this object and frees the allocated resources.
    ~MultilineWrapper();

    inline void setWrappers(NeopixelWrapper *w) { wrappers = w; }
    inline NeopixelWrapper* getWrappers() { return wrappers; }

    inline void setNumWrappers(uint8_t num) { wrapperCount = num; }
    inline uint8_t numWrappers() { return wrapperCount; }

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
    void fill(int32_t color, uint16_t start);
    void fill(int32_t color, uint16_t start, uint16_t count);

    /// (1) Calls the show method of all underlying wrapper objects.
    /// (2) Calls the begin method of all underlying wrapper objects.
    /// (3) Calls the clear method of all underlying wrapper objects.
    void show();
    void begin();
    void clear();

    //// ---- Effects ---- ////

    void runner(int32_t color);
    void blinker(int32_t on, int32_t off);
};

#endif
