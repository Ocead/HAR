//
// Created by Johannes on 16.08.2020.
//

#include <chrono>
#include <random>
#include <thread>

#include <har.hpp>
#include <har/gui.hpp>

#include "har/duino.hpp"

#if defined(__GNUC__)
#define WEAK __attribute__((weak))
#elif defined(_MSC_VER)
#define WEAK __declspec(selectany)
#else
#define WEAK
#endif

extern "C" {

int WEAK main(int argc, char * argv[], char * envp[]);

}

/// \brief Provides a static global instance of <tt>har::duino</tt>
/// \return A static global instance of <tt>har::duino</tt>
inline har::duino & rt() {
    static har::duino rt{ };
    return rt;
}

/// \brief Weak definition of an entry function
/// \param argc Number of command line arguments
/// \param argv Command line arguments
/// \param envp Environment variables
/// \return Return code of the process
int WEAK main(int argc, char * argv[], char * envp[]) {
    har::simulation sim{ argc, argv, envp };
    har::gui gui{ };

    /*Include parts*/ {
        sim.include_part(har::duino::parts::empty());

        sim.include_part(har::duino::parts::push_button());
        sim.include_part(har::duino::parts::switch_button());
        sim.include_part(har::duino::parts::lamp());
        sim.include_part(har::duino::parts::rgb_led());
        sim.include_part(har::duino::parts::seven_segment());

        sim.include_part(har::duino::parts::proximity_sensor());
        sim.include_part(har::duino::parts::color_sensor());
        sim.include_part(har::duino::parts::movement_sensor());

        sim.include_part(har::duino::parts::motor());
        sim.include_part(har::duino::parts::conveyor_belt());
        sim.include_part(har::duino::parts::thread_rod());

        sim.include_part(har::duino::parts::producer());
        sim.include_part(har::duino::parts::destructor());

        sim.include_part(har::duino::parts::box_cargo());

        sim.include_part(har::duino::parts::digital_pin());
        sim.include_part(har::duino::parts::analog_pin());
        sim.include_part(har::duino::parts::constant_pin());
        sim.include_part(har::duino::parts::pwm_pin());
        sim.include_part(har::duino::parts::serial_pin());

        sim.include_part(har::duino::parts::smd_button());
        sim.include_part(har::duino::parts::smd_led());
        sim.include_part(har::duino::parts::timer());

        sim.include_part(har::duino::parts::dummy_pin());
        sim.include_part(har::duino::parts::keying_pin());
    }

    /*Add participants*/ {
        sim.attach(rt()); //setup will be called here
        sim.attach(gui);
    }

    //Set callback for end of simulation
    std::atomic<har::bool_t> exit{ false };
    sim.call_on_exit([&]() {
        //Abort while loop for loop
        exit.store(true, std::memory_order_release);
    });

    sim.commence();

    //Call loop as long as the simulation is running
    debug_log("Start calling loop()");
    while (!exit.load(std::memory_order_acquire)) {
        loop();
    }
    debug_log("Done calling loop()");
}

#include <Arduino.h>

void pinMode(uint8_t pin, uint8_t mode) {
    rt().pinMode(pin, mode);
}

void digitalWrite(uint8_t pin, uint8_t val) {
    rt().digitalWrite(pin, val);
}

int digitalRead(uint8_t pin) {
    return rt().digitalRead(pin);
}

int analogRead(uint8_t pin) {
    return rt().analogRead(pin);
}

void analogReference(uint8_t mode) {
    rt().analogReference(mode);
}

void analogWrite(uint8_t pin, int val) {
    rt().analogWrite(pin, val);
}

void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void delayMicroseconds(unsigned int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

unsigned long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(har::clock::now() - rt().start()).count();
}

unsigned long micros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(har::clock::now() - rt().start()).count();
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    //TODO: Implement
    return 0u;
}

unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout) {
    //TODO: Implement
    return 0u;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
    //TODO: Implement;
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    //TODO: Implement
    return 0u;
}

uint8_t digitalPinToInterrupt(uint8_t pin) {
    return har::duino::digitalPinToInterrupt(pin);
}

void attachInterrupt(uint8_t interruptNum, void (* userFunc)(), int mode) {
    rt().attachInterrupt(interruptNum, userFunc, mode);
}

void detachInterrupt(uint8_t interruptNum) {
    rt().detachInterrupt(interruptNum);
}

std::mt19937_64 & random_base() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    return gen;
}

long random_max(long max) {
    return std::uniform_int_distribution<>(0, max)(random_base());
}

long random_min_max(long min, long max) {
    return std::uniform_int_distribution<>(min, max)(random_base());
}

void randomSeed([[maybe_unused]] unsigned long seed) {

}

long map(long value, long fromLow, long fromHigh, long toLow, long toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
