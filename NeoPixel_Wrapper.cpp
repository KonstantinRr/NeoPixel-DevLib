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

#include "NeoPixel_Wrapper.h"

#define INCLUDE_BLINKER 1
#define INCLUDE_RUNNER 1
#define INCLUDE_COLOR_CHANGER 1

NeopixelWrapper::NeopixelWrapper(
    uint16_t pixels, uint16_t pin, neoPixelType flags, bool inverse
) : Adafruit_NeoPixel(pixels, pin, flags), inverse(inverse)
{

}

uint16_t NeopixelWrapper::getIndex(uint16_t index)
{
    return inverse ?
        numPixels() - 1 - index : index;
}

uint8_t* NeopixelWrapper::getPointer(uint16_t vindex)
{
    uint16_t n = getIndex(vindex); 
    return &pixels[n * (isRGB() ? 3 : 4)];
}

void NeopixelWrapper::setPixelColor(
    uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    setPixelColor(n, r, g, b, 0);
}

void NeopixelWrapper::setPixelColor(
    uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    uint8_t *stripPointer = getPointer(n);
    stripPointer[rOffset] = r;
    stripPointer[gOffset] = g;
    stripPointer[bOffset] = b;
    if (!isRGB()) stripPointer[wOffset] = w;
}

void NeopixelWrapper::setPixelColor(uint16_t n, uint32_t c)
{
    setPixelColor(n,
        (uint8_t)(c >> 16),
        (uint8_t)(c >>  8),
        (uint8_t)(c)
    );
}

void NeopixelWrapper::fill(int32_t c, int16_t start, uint16_t count) {
    // Checks the boundaries
    int16_t end = min(start + (int16_t)count, (int16_t)numPixels());
    for (int16_t i = max(start, 0); i < end; i++)
    {
        setPixelColor(i, c);
    }
}

void NeopixelWrapper::fill(int32_t color, int16_t start) {
    fill(color, start, numPixels() - (uint16_t)start);
}

void NeopixelWrapper::fill(int32_t color) {
    Adafruit_NeoPixel::fill(color);
}

//// ---- MultilineWrapper ---- ////

MultilineWrapper::MultilineWrapper(
    NeopixelWrapper *wrappers, uint8_t stripCount)
{
    indexMap = nullptr;
    setWrappers(wrappers, stripCount);
}

void MultilineWrapper::setWrappers(NeopixelWrapper *pwrappers, uint8_t stripCount)
{
    wrappers = pwrappers;
    wrapperCount = stripCount;

    // Counts the total number of pixels
    pixelCount = 0;
    for (uint8_t i = 0; i < stripCount; i++)
    {
        pixelCount += wrappers[i].numPixels();
    }

    rOffset = wrappers[0].getROffset();
    gOffset = wrappers[0].getGOffset();
    bOffset = wrappers[0].getBOffset();
    wOffset = wrappers[0].getWOffset();

    // Maps each virtual index to the hardware address
    if (indexMap) free(indexMap);
    indexMap = (uint8_t**) malloc(pixelCount * sizeof(uint8_t*));
    if (indexMap) // allocation successful
    {
        uint8_t rawIndex = 0;
        for (uint8_t stripIndex = 0;
            stripIndex < stripCount; stripIndex++)
        {
            for (uint8_t lineIndex = 0;
                lineIndex < wrappers[stripIndex].numPixels(); lineIndex++)
            {
                indexMap[rawIndex] = wrappers[stripIndex]
                    .getPointer(lineIndex);
                rawIndex++;
            }
        }
    }
    else // allocation failed
    {
        pixelCount = 0;
    }
}

MultilineWrapper::~MultilineWrapper()
{
    free(indexMap);
}

void MultilineWrapper::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t* stripIndex = indexMap[n];
    stripIndex[rOffset] = r;
    stripIndex[gOffset] = g;
    stripIndex[bOffset] = b;
    if(!isRGB()) stripIndex[wOffset] = 0;
}

void MultilineWrapper::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    uint8_t* stripIndex = indexMap[n];
    stripIndex[rOffset] = r;
    stripIndex[gOffset] = g;
    stripIndex[bOffset] = b;
    if(!isRGB()) stripIndex[wOffset] = w;
}

void MultilineWrapper::setPixelColor(uint16_t n, uint32_t c)
{
    uint8_t* stripIndex = indexMap[n];
    stripIndex[rOffset] = (uint8_t)(c >> 16);
    stripIndex[gOffset] = (uint8_t)(c >>  8);
    stripIndex[bOffset] = (uint8_t)(c);
    if(!isRGB()) stripIndex[wOffset] = 0;
}

void MultilineWrapper::fill(int32_t c)
{
    int8_t r = (uint8_t)(c >> 16);
    int8_t g = (uint8_t)(c >>  8);
    int8_t b = (uint8_t)(c);
    for (uint16_t i = 0; i < pixelCount; i++)
    {
        setPixelColor(i, r, g, b);
    }
}

void MultilineWrapper::fill(int32_t c, int16_t start)
{
    int8_t r = (uint8_t)(c >> 16);
    int8_t g = (uint8_t)(c >>  8);
    int8_t b = (uint8_t)(c);
    for (int16_t i = start >= 0 ? start : 0; i < pixelCount; i++)
    {
        setPixelColor(i, r, g, b);
    }
}

void MultilineWrapper::fill(int32_t c, int16_t start, uint16_t count)
{
    int8_t r = (uint8_t)(c >> 16);
    int8_t g = (uint8_t)(c >>  8);
    int8_t b = (uint8_t)(c);
    int16_t end = min(start + (int16_t)count, (int16_t)pixelCount);
    for (int16_t i = max(start, 0); i < end; i++)
    {
        setPixelColor(i, r, g, b);
    }
}

void MultilineWrapper::begin()
{
    for (uint8_t i = 0; i < wrapperCount; i++)
    {
        wrappers[i].begin();
    }
} 

void MultilineWrapper::show()
{
    for (uint8_t i = 0; i < wrapperCount; i++)
    {
        wrappers[i].show();
    }
}

void MultilineWrapper::clear()
{
    for (uint16_t i = 0; i < pixelCount; i++)
    {
        setPixelColor(i, 0, 0, 0);
    }
}

//// ---- Effects ---- ////

#if INCLUDE_BLINKER

void Blinker::update()
{
    if (state & 0x1)
    {
        wrapper->fill(colorOn);
    }
    else
    {
        wrapper->fill(colorOff);
    }
    state++;
}

#endif

#if INCLUDE_RUNNER

void Runner::update()
{
    if (direction > 0) {
        state = (state + direction);
        if (state >= wrapper->numPixels()) state = 0;
    } else {
        state = (state - 1);
        if (state < 0) state = wrapper->numPixels() - 1;
    }
    wrapper->fill(color, state, length);
    if (direction > 0)
    {
        int16_t overlap = state + length -
            (int16_t)wrapper->numPixels();
        if (overlap > 0) {
            wrapper->fill(color, 0, overlap);
        }
    }
    else {
        // TODO implement
    }
}

#endif

#if INCLUDE_COLOR_CHANGER

void ColorChanger::update()
{
    int8_t rBegin = (uint8_t)(colorStart >> 16);
    int8_t gBegin = (uint8_t)(colorStart >>  8);
    int8_t bBegin = (uint8_t)(colorStart);

    int8_t rEnd = (uint8_t)(colorEnd >> 16);
    int8_t gEnd = (uint8_t)(colorEnd >>  8);
    int8_t bEnd = (uint8_t)(colorEnd);

    for (uint8_t i = 0; i < 255; i++)
    {
        uint8_t val = Adafruit_NeoPixel::sine8(i);

        uint8_t r = (rBegin * val) >> 8;
        uint8_t g = (gBegin * val) >> 8;
        uint8_t b = (bBegin * val) >> 8;
    }
}

#endif
