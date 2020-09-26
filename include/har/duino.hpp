//
// Created by Johannes on 28.08.2020.
//

#ifndef HAR_DUINO_HPP
#define HAR_DUINO_HPP

#include <atomic>

#include <har/program.hpp>

extern "C" {

extern void setup();

extern void loop();

};

namespace har {

    /// \brief Provides a runtime for Arduino sketches
    class duino : public har::program {
    private:
        std::chrono::time_point<har::clock> _start; ///<Timepoint of start of the runtime
        std::atomic<uint_t> _setup;

        /// \brief Maps an Arduino pin number to the appropriate "digital pin" cell in the corresponding model
        /// \param [in] ctx Participant context
        /// \param [in] pin Pin number
        /// \return The appropriate cell
        static full_grid_cell map_cell_digital(context & ctx, uint8_t pin);

        /// \brief Maps an Arduino pin number to the appropriate "analog pin" cell in the corresponding model
        /// \param [in] ctx Participant context
        /// \param [in] pin Pin number
        /// \return The appropriate cell
        static full_grid_cell map_cell_analog(context & ctx, uint8_t pin);

        /// \brief Maps an Arduino interrupt number to the appropriate cell in the corresponding model
        /// \param [in] ctx Participant context
        /// \param [in] pin Pin number
        /// \return The appropriate cell
        static full_grid_cell map_cell_interrupt(context & ctx, uint8_t num);

    public:
        class parts {
        public:
            static part empty(part_h offset = PART[0]);

            static part push_button(part_h offset = PART[0]);

            static part switch_button(part_h offset = PART[0]);

            static part lamp(part_h offset = PART[0]);

            static part rgb_led(part_h offset = PART[0]);

            static part seven_segment(part_h offset = PART[0]);

            static part proximity_sensor(part_h offset = PART[0]);

            static part color_sensor(part_h offset = PART[0]);

            static part movement_sensor(part_h offset = PART[0]);

            static part motor(part_h offset = PART[0]);

            static part conveyor_belt(part_h offset = PART[0]);

            static part thread_rod(part_h offset = PART[0]);

            static part producer(part_h offset = PART[0]);

            static part destructor(part_h offset = PART[0]);

            static part box_cargo(part_h offset = PART[0]);

            static part digital_pin(part_h offset = PART[0]);

            static part analog_pin(part_h offset = PART[0]);

            static part constant_pin(part_h offset = PART[0]);

            static part pwm_pin(part_h offset = PART[0]);

            static part serial_pin(part_h offset = PART[0]);

            static part smd_button(part_h offset = PART[0]);

            static part smd_led(part_h offset = PART[0]);

            static part timer(part_h offset = PART[0]);

            static part dummy_pin(part_h offset = PART[0]);

            static part keying_pin(part_h offset = PART[0]);
        };

        /// \brief Default constructor
        duino();

        /// \brief Sets up the duino runtime and loads the Arduino Uno model
        /// \param [in] argc Number of command line arguments
        /// \param [in] argv Command line arguments
        /// \param [in] envp Environment variables
        void on_attach(int argc, char * const * argv, char * const * envp) override;

        /// \brief Sets the start timepoint for the runtime and calls the <tt>setup</tt> function
        void on_model_loaded() override;

        /// \brief Creates an exclusive context, if the participant is attached.
        /// Terminates the calling thread otherwise.
        /// \return A context
        program::context request_or_terminate();

        /// \brief Returns the time point the runtime was started
        /// \return The time point the runtime was started
        [[nodiscard]]
        const decltype(_start) & start() const;

        void maybe_setup();

        /// \brief Reads the value from a specified digital pin, either <tt>HIGH</tt> or <tt>LOW</tt>.
        /// \param [in] pin the Arduino pin number you want to read
        /// \return <tt>HIGH</tt> or <tt>LOW</tt>
        int digitalRead(uint8_t pin);

        /// \brief Write a <tt>HIGH</tt> or <tt>LOW</tt> value to a digital pin.
        /// \param [in] pin the Arduino pin number
        /// \param [in] val <tt>HIGH</tt> or <tt>LOW</tt>
        void digitalWrite(uint8_t pin, uint8_t val);

        /// \brief Configures the specified pin to behave either as an input or an output.
        /// See the Digital Pins page for details on the functionality of the pins.
        /// \param [in] pin the Arduino pin number to set the mode of
        /// \param [in] mode <tt>INPUT</tt>, <tt>OUTPUT</tt>, or <tt>INPUT_PULLUP</tt>
        void pinMode(uint8_t pin, uint8_t mode);

        /// \brief Reads the value from the specified analog pin
        /// \param [in] pin the name of the analog input pin to read from
        /// \return The analog reading on the pin. <br/>
        /// Although it is limited to the resolution of the analog to digital converter (0-1023 for 10 bits)
        int analogRead(uint8_t pin);

        /// \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range)
        /// \param [in] type which type of reference to use
        void analogReference(uint8_t type);

        /// \brief Writes an analog value (PWM wave) to a pin
        /// \param [in] pin the Arduino pin to write to
        /// \param [in] val the duty cycle: between 0 (always off) and 255 (always on)
        void analogWrite(uint8_t pin, int val);

        ///
        /// \param [in] pin
        /// \return
        static uint8_t digitalPinToInterrupt(uint8_t pin);

        ///
        /// \param [in] interruptNum the number of the interrupt
        /// \param [in] userFunc the ISR to call when the interrupt occurs
        /// \param [in] mode defines when the interrupt should be triggered <br/>
        ///                  <ul>
        ///                    <li><tt>LOW</tt>: to trigger the interrupt whenever the pin is low</li>
        ///                    <li><tt>CHANGE</tt>: to trigger the interrupt whenever the pin changes value</li>
        ///                    <li><tt>RISING</tt>: to trigger when the pin goes from low to high</li>
        ///                    <li><tt>FALLING</tt>: for when the pin goes from high to low</li>
        ///                  </ul>
        void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);

        /// \brief Turns off the given interrupt
        /// \param [in] interruptNum the number of the interrupt to disable
        void detachInterrupt(uint8_t interruptNum);

        /// \brief Default destructor
        ~duino() noexcept override;
    };
}

#endif //HAR_DUINO_HPP
