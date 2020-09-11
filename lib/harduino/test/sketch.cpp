//
// Created by Johannes on 18.07.2020.
//

#include <har.hpp>

#include <har/duino.hpp>
#include <har/gui.hpp>

int main(int argc, char * argv[], char * envp[]) {
    static har::simulation sim{ argc, argv };
    static har::program prog{ };
    static har::gui gui{ };
    static std::mutex latch{ };

    latch.lock();

    sim.call_on_exit([&] {
        latch.unlock();
    });

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
        sim.attach(prog);
        sim.attach(gui);
    }

    sim.commence();

    latch.lock();
}
