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

NeopixelWrapper::NeopixelWrapper(
    int8_t pixels, int8_t pin,
    neoPixelType flags, bool inverse
) : Adafruit_NeoPixel(pixels, pin, flags),
    inverse(inverse)
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
    return isRGB() ?
        &pixels[n * 3] :
        &pixels[n * 4];
}

void NeopixelWrapper::setPixelColor(
    uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t index = getIndex(n);
    Adafruit_NeoPixel::setPixelColor(index, r, g, b);
}

void NeopixelWrapper::setPixelColor(
    uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    uint16_t index = getIndex(n);
    Adafruit_NeoPixel::setPixelColor(index, r, g, b, w);
}

void NeopixelWrapper::setPixelColor(uint16_t n, uint32_t c) {
    uint16_t index = getIndex(n);
    Adafruit_NeoPixel::setPixelColor(index, c);
}

void NeopixelWrapper::fill(int32_t color, uint16_t start, uint16_t count) {
    uint16_t reverseStart = getIndex(start + count);
    Adafruit_NeoPixel::fill(color, start, count);
}

void NeopixelWrapper::fill(int32_t color, uint16_t start) {
    if (inverse) {
        Adafruit_NeoPixel::fill(color, 0, numPixels() - 1 - start);
    } else {
        Adafruit_NeoPixel::fill(color, start);
    }
}

void NeopixelWrapper::fill(int32_t color) {
    Adafruit_NeoPixel::fill(color);
}

//// ---- MultilineWrapper ---- ////

MultilineWrapper::MultilineWrapper(
    NeopixelWrapper *wrappers, uint8_t stripCount
) : wrappers(wrappers), wrapperCount(stripCount)
{
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

    // Maps each virtual index to the 
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

void MultilineWrapper::fill(int32_t c, uint16_t start)
{
    int8_t r = (uint8_t)(c >> 16);
    int8_t g = (uint8_t)(c >>  8);
    int8_t b = (uint8_t)(c);
    for (uint16_t i = start; i < pixelCount; i++)
    {
        setPixelColor(i, r, g, b);
    }
}

void MultilineWrapper::fill(int32_t c, uint16_t start, uint16_t count)
{
    int8_t r = (uint8_t)(c >> 16);
    int8_t g = (uint8_t)(c >>  8);
    int8_t b = (uint8_t)(c);
    for (uint16_t i = start; i < start + count; i++)
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

void MultilineWrapper::runner(int32_t color)
{
    for (uint16_t i = 0; i < pixelCount; i++)
    {
        clear();
        setPixelColor(i, color);
        show();
    }
}

void MultilineWrapper::blinker(int32_t on, int32_t off)
{
    clear();
    fill(on);
    clear();
    fill(off);
}