//
// Created by Johannes on 03.11.2020.
//

#ifndef HAR_PROCESS_TAB_HPP
#define HAR_PROCESS_TAB_HPP

#include <functional>
#include <map>

#include <har/cell_base.hpp>
#include <har/types.hpp>

namespace har {

    enum process : ushort_t {
        NONE = 0u,
        CYCLE = 1u << 0u,
        MOVE = 1u << 1u
    };

    struct cell_tab {
    public:
        std::reference_wrapper<cell_base> cell;
        process status;

        cell_tab(cell_base & cell, process status) : cell(cell), status(status) {

        }
    };

    /// \brief Keeps track which logic to apply to cells in a cycle
    struct process_tab {
    private:
        std::map<cell_h, cell_tab> _active; ///<Cycling or moving cells
        std::map<cell_h, std::reference_wrapper<cell_base>> _waking; ///<Cells that start cycling next cycle
        std::map<cell_h, std::reference_wrapper<cell_base>> _tiring; ///<Cells that stop cycling next cycle
        std::map<cell_h, std::reference_wrapper<cell_base>> _starting; ///<Cells that start moving next cycle
        std::map<cell_h, std::reference_wrapper<cell_base>> _halting; ///<Cells that stop moving next cycle
        std::map<cell_h, std::reference_wrapper<cell_base>> _inactive; ///<Cells that neither cycle of move

    public:
        /// \brief Standard constructor
        process_tab();

        /// \brief Wakes up a cell
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] clb Reference to the cell
        void wake(const cell_h & hnd, cell_base & clb);

        /// \brief Retires a cell
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] clb Reference to the cell
        void tire(const cell_h & hnd, cell_base & clb);

        /// \brief Starts moving of a cell
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] clb Reference to the cell
        void start(const cell_h & hnd, cell_base & clb);

        /// \brief Stops moving of a cell
        ///
        /// \param [in] hnd Handle of the cell
        /// \param [in] clb Reference to the cell
        void halt(const cell_h & hnd, cell_base & clb);

        /// \brief Applies changes of the process tab
        void apply();

        /// \brief Removes a cell from the process tab
        ///
        /// \param [in] hnd Handle of the cell
        void remove(const cell_h & hnd);

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_active) & get_active() const;

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_waking) & get_waking() const;

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_tiring) & get_tiring() const;

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_starting) & get_starting() const;

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_halting) & get_halting() const;

        /// \brief
        ///
        /// \return
        [[nodiscard]]
        const decltype(_inactive) & get_inactive() const;

        /// \brief Standard destructor
        ~process_tab();
    };

}

#endif //HAR_PROCESS_TAB_HPP
