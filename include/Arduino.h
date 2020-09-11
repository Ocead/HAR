//
// Created by Johannes on 29.07.2020.
//

#ifndef HAR_ARDUINO_H
#define HAR_ARDUINO_H

#ifndef Arduino_h
#define Arduino_h

#ifndef __cplusplus
typedef _Bool bool;
typedef _Bool boolean;
#else
typedef bool boolean;
#endif

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned char uint8_t;

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define DEFAULT 0
#define EXTERNAL 1
#define INTERNAL1V1 2
#define INTERNAL INTERNAL1V1

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt, low, high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))
#define getRandom(_1, _2, NAME, ...) NAME
#define random(...) getRandom(__VA_ARGS__, randomMinMax, randomMax)(__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

//region Sketch forward declaration

void setup();

void loop();

//endregion

//region Pins

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#define SS   (10u)
#define MOSI (11u)
#define MISO (12u)
#define SCK  (13u)

#define SDA (18u)
#define SCL (19u)

#define LED_BUILTIN (13u)

#define A0 (14u)
#define A1 (15u)
#define A2 (16u)
#define A3 (17u)
#define A4 (18u)
#define A5 (19u)
#define A6 (20u)
#define A7 (21u)

#endif //Pins_Arduino_h

//endregion

//region I/O

int digitalRead(uint8_t pin);

void digitalWrite(uint8_t pin, uint8_t val);

void pinMode(uint8_t pin, uint8_t mode);

int analogRead(uint8_t pin);

void analogReference(uint8_t mode);

void analogWrite(uint8_t pin, int val);

//endregion

//region Timing

void delay(unsigned long ms);

void delayMicroseconds(unsigned int us);

unsigned long millis();

unsigned long micros();

//endregion

//region Math

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh);

double cos(double rad);

double sin(double rad);

double tan(double rad);

//endregion

//region Random

long randomMax(long max);

long randomMinMax(long min, long max);

void randomSeed(unsigned long seed);

//endregion

//region Interrupt

uint8_t digitalPinToInterrupt(uint8_t pin);

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);

void detachInterrupt(uint8_t interruptNum);

//endregion

#ifdef __cplusplus
}
#endif

#endif //Arduino_h

//region Binary

//The saving grace is that Arduino binary literals only support up to 8 digits

#ifndef Binary_h
#define Binary_h

#define B00000000 0u
#define B00000001 1u
#define B00000010 2u
#define B00000011 3u
#define B00000100 4u
#define B00000101 5u
#define B00000110 6u
#define B00000111 7u
#define B00001000 8u
#define B00001001 9u
#define B00001010 10u
#define B00001011 11u
#define B00001100 12u
#define B00001101 13u
#define B00001110 14u
#define B00001111 15u
#define B00010000 16u
#define B00010001 17u
#define B00010010 18u
#define B00010011 19u
#define B00010100 20u
#define B00010101 21u
#define B00010110 22u
#define B00010111 23u
#define B00011000 24u
#define B00011001 25u
#define B00011010 26u
#define B00011011 27u
#define B00011100 28u
#define B00011101 29u
#define B00011110 30u
#define B00011111 31u
#define B00100000 32u
#define B00100001 33u
#define B00100010 34u
#define B00100011 35u
#define B00100100 36u
#define B00100101 37u
#define B00100110 38u
#define B00100111 39u
#define B00101000 40u
#define B00101001 41u
#define B00101010 42u
#define B00101011 43u
#define B00101100 44u
#define B00101101 45u
#define B00101110 46u
#define B00101111 47u
#define B00110000 48u
#define B00110001 49u
#define B00110010 50u
#define B00110011 51u
#define B00110100 52u
#define B00110101 53u
#define B00110110 54u
#define B00110111 55u
#define B00111000 56u
#define B00111001 57u
#define B00111010 58u
#define B00111011 59u
#define B00111100 60u
#define B00111101 61u
#define B00111110 62u
#define B00111111 63u
#define B01000000 64u
#define B01000001 65u
#define B01000010 66u
#define B01000011 67u
#define B01000100 68u
#define B01000101 69u
#define B01000110 70u
#define B01000111 71u
#define B01001000 72u
#define B01001001 73u
#define B01001010 74u
#define B01001011 75u
#define B01001100 76u
#define B01001101 77u
#define B01001110 78u
#define B01001111 79u
#define B01010000 80u
#define B01010001 81u
#define B01010010 82u
#define B01010011 83u
#define B01010100 84u
#define B01010101 85u
#define B01010110 86u
#define B01010111 87u
#define B01011000 88u
#define B01011001 89u
#define B01011010 90u
#define B01011011 91u
#define B01011100 92u
#define B01011101 93u
#define B01011110 94u
#define B01011111 95u
#define B01100000 96u
#define B01100001 97u
#define B01100010 98u
#define B01100011 99u
#define B01100100 100u
#define B01100101 101u
#define B01100110 102u
#define B01100111 103u
#define B01101000 104u
#define B01101001 105u
#define B01101010 106u
#define B01101011 107u
#define B01101100 108u
#define B01101101 109u
#define B01101110 110u
#define B01101111 111u
#define B01110000 112u
#define B01110001 113u
#define B01110010 114u
#define B01110011 115u
#define B01110100 116u
#define B01110101 117u
#define B01110110 118u
#define B01110111 119u
#define B01111000 120u
#define B01111001 121u
#define B01111010 122u
#define B01111011 123u
#define B01111100 124u
#define B01111101 125u
#define B01111110 126u
#define B01111111 127u
#define B10000000 128u
#define B10000001 129u
#define B10000010 130u
#define B10000011 131u
#define B10000100 132u
#define B10000101 133u
#define B10000110 134u
#define B10000111 135u
#define B10001000 136u
#define B10001001 137u
#define B10001010 138u
#define B10001011 139u
#define B10001100 140u
#define B10001101 141u
#define B10001110 142u
#define B10001111 143u
#define B10010000 144u
#define B10010001 145u
#define B10010010 146u
#define B10010011 147u
#define B10010100 148u
#define B10010101 149u
#define B10010110 150u
#define B10010111 151u
#define B10011000 152u
#define B10011001 153u
#define B10011010 154u
#define B10011011 155u
#define B10011100 156u
#define B10011101 157u
#define B10011110 158u
#define B10011111 159u
#define B10100000 160u
#define B10100001 161u
#define B10100010 162u
#define B10100011 163u
#define B10100100 164u
#define B10100101 165u
#define B10100110 166u
#define B10100111 167u
#define B10101000 168u
#define B10101001 169u
#define B10101010 170u
#define B10101011 171u
#define B10101100 172u
#define B10101101 173u
#define B10101110 174u
#define B10101111 175u
#define B10110000 176u
#define B10110001 177u
#define B10110010 178u
#define B10110011 179u
#define B10110100 180u
#define B10110101 181u
#define B10110110 182u
#define B10110111 183u
#define B10111000 184u
#define B10111001 185u
#define B10111010 186u
#define B10111011 187u
#define B10111100 188u
#define B10111101 189u
#define B10111110 190u
#define B10111111 191u
#define B11000000 192u
#define B11000001 193u
#define B11000010 194u
#define B11000011 195u
#define B11000100 196u
#define B11000101 197u
#define B11000110 198u
#define B11000111 199u
#define B11001000 200u
#define B11001001 201u
#define B11001010 202u
#define B11001011 203u
#define B11001100 204u
#define B11001101 205u
#define B11001110 206u
#define B11001111 207u
#define B11010000 208u
#define B11010001 209u
#define B11010010 210u
#define B11010011 211u
#define B11010100 212u
#define B11010101 213u
#define B11010110 214u
#define B11010111 215u
#define B11011000 216u
#define B11011001 217u
#define B11011010 218u
#define B11011011 219u
#define B11011100 220u
#define B11011101 221u
#define B11011110 222u
#define B11011111 223u
#define B11100000 224u
#define B11100001 225u
#define B11100010 226u
#define B11100011 227u
#define B11100100 228u
#define B11100101 229u
#define B11100110 230u
#define B11100111 231u
#define B11101000 232u
#define B11101001 233u
#define B11101010 234u
#define B11101011 235u
#define B11101100 236u
#define B11101101 237u
#define B11101110 238u
#define B11101111 239u
#define B11110000 240u
#define B11110001 241u
#define B11110010 242u
#define B11110011 243u
#define B11110100 244u
#define B11110101 245u
#define B11110110 246u
#define B11110111 247u
#define B11111000 248u
#define B11111001 249u
#define B11111010 250u
#define B11111011 251u
#define B11111100 252u
#define B11111101 253u
#define B11111110 254u
#define B11111111 255u

#define B0000000 B00000000
#define B0000001 B00000001
#define B0000010 B00000010
#define B0000011 B00000011
#define B0000100 B00000100
#define B0000101 B00000101
#define B0000110 B00000110
#define B0000111 B00000111
#define B0001000 B00001000
#define B0001001 B00001001
#define B0001010 B00001010
#define B0001011 B00001011
#define B0001100 B00001100
#define B0001101 B00001101
#define B0001110 B00001110
#define B0001111 B00001111
#define B0010000 B00010000
#define B0010001 B00010001
#define B0010010 B00010010
#define B0010011 B00010011
#define B0010100 B00010100
#define B0010101 B00010101
#define B0010110 B00010110
#define B0010111 B00010111
#define B0011000 B00011000
#define B0011001 B00011001
#define B0011010 B00011010
#define B0011011 B00011011
#define B0011100 B00011100
#define B0011101 B00011101
#define B0011110 B00011110
#define B0011111 B00011111
#define B0100000 B00100000
#define B0100001 B00100001
#define B0100010 B00100010
#define B0100011 B00100011
#define B0100100 B00100100
#define B0100101 B00100101
#define B0100110 B00100110
#define B0100111 B00100111
#define B0101000 B00101000
#define B0101001 B00101001
#define B0101010 B00101010
#define B0101011 B00101011
#define B0101100 B00101100
#define B0101101 B00101101
#define B0101110 B00101110
#define B0101111 B00101111
#define B0110000 B00110000
#define B0110001 B00110001
#define B0110010 B00110010
#define B0110011 B00110011
#define B0110100 B00110100
#define B0110101 B00110101
#define B0110110 B00110110
#define B0110111 B00110111
#define B0111000 B00111000
#define B0111001 B00111001
#define B0111010 B00111010
#define B0111011 B00111011
#define B0111100 B00111100
#define B0111101 B00111101
#define B0111110 B00111110
#define B0111111 B00111111
#define B1000000 B01000000
#define B1000001 B01000001
#define B1000010 B01000010
#define B1000011 B01000011
#define B1000100 B01000100
#define B1000101 B01000101
#define B1000110 B01000110
#define B1000111 B01000111
#define B1001000 B01001000
#define B1001001 B01001001
#define B1001010 B01001010
#define B1001011 B01001011
#define B1001100 B01001100
#define B1001101 B01001101
#define B1001110 B01001110
#define B1001111 B01001111
#define B1010000 B01010000
#define B1010001 B01010001
#define B1010010 B01010010
#define B1010011 B01010011
#define B1010100 B01010100
#define B1010101 B01010101
#define B1010110 B01010110
#define B1010111 B01010111
#define B1011000 B01011000
#define B1011001 B01011001
#define B1011010 B01011010
#define B1011011 B01011011
#define B1011100 B01011100
#define B1011101 B01011101
#define B1011110 B01011110
#define B1011111 B01011111
#define B1100000 B01100000
#define B1100001 B01100001
#define B1100010 B01100010
#define B1100011 B01100011
#define B1100100 B01100100
#define B1100101 B01100101
#define B1100110 B01100110
#define B1100111 B01100111
#define B1101000 B01101000
#define B1101001 B01101001
#define B1101010 B01101010
#define B1101011 B01101011
#define B1101100 B01101100
#define B1101101 B01101101
#define B1101110 B01101110
#define B1101111 B01101111
#define B1110000 B01110000
#define B1110001 B01110001
#define B1110010 B01110010
#define B1110011 B01110011
#define B1110100 B01110100
#define B1110101 B01110101
#define B1110110 B01110110
#define B1110111 B01110111
#define B1111000 B01111000
#define B1111001 B01111001
#define B1111010 B01111010
#define B1111011 B01111011
#define B1111100 B01111100
#define B1111101 B01111101
#define B1111110 B01111110
#define B1111111 B01111111

#define B000000 B00000000
#define B000001 B00000001
#define B000010 B00000010
#define B000011 B00000011
#define B000100 B00000100
#define B000101 B00000101
#define B000110 B00000110
#define B000111 B00000111
#define B001000 B00001000
#define B001001 B00001001
#define B001010 B00001010
#define B001011 B00001011
#define B001100 B00001100
#define B001101 B00001101
#define B001110 B00001110
#define B001111 B00001111
#define B010000 B00010000
#define B010001 B00010001
#define B010010 B00010010
#define B010011 B00010011
#define B010100 B00010100
#define B010101 B00010101
#define B010110 B00010110
#define B010111 B00010111
#define B011000 B00011000
#define B011001 B00011001
#define B011010 B00011010
#define B011011 B00011011
#define B011100 B00011100
#define B011101 B00011101
#define B011110 B00011110
#define B011111 B00011111
#define B100000 B00100000
#define B100001 B00100001
#define B100010 B00100010
#define B100011 B00100011
#define B100100 B00100100
#define B100101 B00100101
#define B100110 B00100110
#define B100111 B00100111
#define B101000 B00101000
#define B101001 B00101001
#define B101010 B00101010
#define B101011 B00101011
#define B101100 B00101100
#define B101101 B00101101
#define B101110 B00101110
#define B101111 B00101111
#define B110000 B00110000
#define B110001 B00110001
#define B110010 B00110010
#define B110011 B00110011
#define B110100 B00110100
#define B110101 B00110101
#define B110110 B00110110
#define B110111 B00110111
#define B111000 B00111000
#define B111001 B00111001
#define B111010 B00111010
#define B111011 B00111011
#define B111100 B00111100
#define B111101 B00111101
#define B111110 B00111110
#define B111111 B00111111

#define B00000 B00000000
#define B00001 B00000001
#define B00010 B00000010
#define B00011 B00000011
#define B00100 B00000100
#define B00101 B00000101
#define B00110 B00000110
#define B00111 B00000111
#define B01000 B00001000
#define B01001 B00001001
#define B01010 B00001010
#define B01011 B00001011
#define B01100 B00001100
#define B01101 B00001101
#define B01110 B00001110
#define B01111 B00001111
#define B10000 B00010000
#define B10001 B00010001
#define B10010 B00010010
#define B10011 B00010011
#define B10100 B00010100
#define B10101 B00010101
#define B10110 B00010110
#define B10111 B00010111
#define B11000 B00011000
#define B11001 B00011001
#define B11010 B00011010
#define B11011 B00011011
#define B11100 B00011100
#define B11101 B00011101
#define B11110 B00011110
#define B11111 B00011111

#define B0000 B00000000
#define B0001 B00000001
#define B0010 B00000010
#define B0011 B00000011
#define B0100 B00000100
#define B0101 B00000101
#define B0110 B00000110
#define B0111 B00000111
#define B1000 B00001000
#define B1001 B00001001
#define B1010 B00001010
#define B1011 B00001011
#define B1100 B00001100
#define B1101 B00001101
#define B1110 B00001110
#define B1111 B00001111

#define B000 B00000000
#define B001 B00000001
#define B010 B00000010
#define B011 B00000011
#define B100 B00000100
#define B101 B00000101
#define B110 B00000110
#define B111 B00000111

#define B00 B00000000
#define B01 B00000001
#define B10 B00000010
#define B11 B00000011

#define B0 B00000000
#define B1 B00000001

#endif //Binary_h

//endregion


#endif //HAR_ARDUINO_H
