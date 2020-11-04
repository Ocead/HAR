//
// Created by Johannes on 03.11.2020.
//

#include "logic/process_tab.hpp"

using namespace har;

//region cell_tab

cell_tab::cell_tab(cell_base & cell, process status) : cell(cell), status(status) {

}

//endregion

//region process_tab

process_tab::process_tab() = default;

void process_tab::wake(const gcoords_t & pos, cell_base & clb) {
    _waking.try_emplace(pos, clb);
}

void process_tab::tire(const gcoords_t & pos, cell_base & clb) {
    _tiring.try_emplace(pos, clb);
}

void process_tab::start(const gcoords_t & pos, cell_base & clb) {
    _starting.try_emplace(pos, clb);
}

void process_tab::halt(const gcoords_t & pos, cell_base & clb) {
    _halting.try_emplace(pos, clb);
}

void process_tab::apply() {
    /*Tire*/ {
        for (auto &[hnd, clb] : _tiring) {
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto prc = process(~process::CYCLE);
                {
                    auto &[cl, tab] = it->second;
                    prc = tab = process(tab & prc);
                }
                if (!prc) {
                    _active.erase(it);
                    _inactive.try_emplace(hnd, clb);
                }
            } else {
                _inactive.try_emplace(hnd, clb);
            }
        }
        _tiring.clear();
    }

    /*Halt*/ {
        for (auto &[hnd, clb] : _halting) {
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto prc = process(~process::MOVE);
                {
                    auto &[cl, tab] = it->second;
                    prc = tab = process(tab & prc);
                }
                if (!prc) {
                    _active.erase(it);
                    _inactive.try_emplace(hnd, clb);
                }
            } else {
                _inactive.try_emplace(hnd, clb);
            }
        }
        _halting.clear();
    }

    /*Wake*/ {
        for (auto &[hnd, clb] : _waking) {
            _inactive.erase(hnd);
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto &[cl, tab] = it->second;
                tab = process(tab | process::CYCLE);
            } else {
                _active.try_emplace(hnd, clb, process::CYCLE);
            }
        }
        _waking.clear();
    }

    /*Start*/ {
        for (auto &[hnd, clb] : _starting) {
            _inactive.erase(hnd);
            if (auto it = _active.find(hnd); it != _active.end()) {
                auto &[cl, tab] = it->second;
                tab = process(tab | process::MOVE);
            } else {
                _active.try_emplace(hnd, clb, process::MOVE);
            }
        }
        _starting.clear();
    }
}

void process_tab::remove(const gcoords_t & pos) {
    _active.erase(pos);
    _waking.erase(pos);
    _tiring.erase(pos);
    _starting.erase(pos);
    _halting.erase(pos);
    _inactive.erase(pos);
}

size_t process_tab::size() const {
    return _active.size() + _inactive.size();
}

const decltype(process_tab::_active) & process_tab::get_active() const {
    return _active;
}

const decltype(process_tab::_waking) & process_tab::get_waking() const {
    return _waking;
}

const decltype(process_tab::_tiring) & process_tab::get_tiring() const {
    return _tiring;
}

const decltype(process_tab::_starting) & process_tab::get_starting() const {
    return _starting;
}

const decltype(process_tab::_halting) & process_tab::get_halting() const {
    return _halting;
}

const decltype(process_tab::_inactive) & process_tab::get_inactive() const {
    return _inactive;
}

process_tab::~process_tab() = default;

//endregion
