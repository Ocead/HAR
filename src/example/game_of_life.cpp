//
// Created by Johannes on 23.09.2020.
//

#include <har.hpp>
#include <har/gui.hpp>

using namespace har;

part gol_cell() {
    part pt{ PART[0],
             text("eg:gol_cell"),
             traits::EMPTY_PART |
             traits::COMPONENT_PART |
             traits::BOARD_PART |
             traits::SOLID |
             traits::COLORED,
             text("Game of Life cell") };

    pt.add_entry(entry{ of::TYPE,
                        text("__TYPE"),
                        text("Type"),
                        value(part_h()),
                        ui_access::VISIBLE,
                        serialize::NO_SERIALIZE });

    pt.add_entry(entry{ of::NAME,
                        text("__NAME"),
                        text("Name"),
                        value(string_t()),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE });

    pt.add_entry(entry{ of::COLOR,
                        text("__COLOR"),
                        text("Color"),
                        value(color_t()),
                        ui_access::CHANGEABLE,
                        serialize::SERIALIZE });

    pt.add_entry(entry{ of::VALUE,
                        text("__ALIVE"),
                        text("Alive"),
                        value(bool_t()),
                        ui_access::CHANGEABLE,
                        serialize::NO_SERIALIZE });

    pt.delegates.cycle = [](cell & cl) {
        uint_t count{ 0 };
        bool_t alive{ cl[of::VALUE] };

        auto & gcl = cl.as_grid_cell();

        for (auto dir : direction::cardinal) {
            auto ncl = gcl[dir];
            auto nncl = ncl.as_grid_cell()[cw(dir)];
            if (ncl.is_placed() && ncl.logic() == cl.logic()) {
                if (bool_t(ncl[of::VALUE])) {
                    count++;
                }
                if (nncl.is_placed() && nncl.logic() == cl.logic()) {
                    if (bool_t(ncl.as_grid_cell()[cw(dir)][of::VALUE])) {
                        count++;
                    }
                }
            }
        }

        if (!alive) {
            if (count == 3) {
                cl[of::VALUE] = true;
            }
        } else {
            if (count < 2 || count > 3) {
                cl[of::VALUE] = false;
            }
        }
    };

    pt.delegates.draw = [](cell & cl, image_t & im) {
        if (im.type() == typeid(Glib::RefPtr<Gdk::Pixbuf>)) {
            Cairo::RefPtr<Cairo::Surface> sf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 256, 256);

            if (bool_t(cl[of::VALUE]) || !cl.as_grid_cell().is_placed()) {
                auto cr = Cairo::Context::create(sf);
                auto color = color_t(cl[of::COLOR]);
                cr->set_source_rgba(color.r / 256., color.g / 256., color.b / 256., 1.);
                cr->rectangle(0., 0., 256., 256.);
                cr->fill();
                cr->stroke();
            }

            im = std::make_tuple(sf, uint_t(256u));
        }
    };

    pt.add_visuals({ of::COLOR,
                     of::VALUE });

    return pt;
}

int main(int argc, char * argv[], char * envp[]) {
    static simulation sim{ argc, argv, envp };
    static gui gui{ };

    sim.include_part(gol_cell());

    sim.attach(gui);

    std::mutex lock{ };
    lock.lock();
    sim.call_on_exit([&]() {
        lock.unlock();
    });

    sim.commence();

    lock.lock();
}