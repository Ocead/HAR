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

        cell_tab(cell_base & cell, process status);
    };

    /// \brief Keeps track which logic to apply to cells in a cycle
    struct process_tab {
    private:
        map<gcoords_t, cell_tab> _active; ///<Cycling or moving cells
        map<gcoords_t, std::reference_wrapper<cell_base>> _waking; ///<Cells that start cycling next cycle
        map<gcoords_t, std::reference_wrapper<cell_base>> _tiring; ///<Cells that stop cycling next cycle
        map<gcoords_t, std::reference_wrapper<cell_base>> _starting; ///<Cells that start moving next cycle
        map<gcoords_t, std::reference_wrapper<cell_base>> _halting; ///<Cells that stop moving next cycle
        map<gcoords_t, std::reference_wrapper<cell_base>> _inactive; ///<Cells that neither cycle nor move

    public:
        /// \brief Standard constructor
        process_tab();

        /// \brief Wakes up a cell
        ///
        /// \param [in] pos Handle of the cell
        /// \param [in] clb Reference to the cell
        void wake(const gcoords_t & pos, cell_base & clb);

        /// \brief Retires a cell
        ///
        /// \param [in] pos Handle of the cell
        /// \param [in] clb Reference to the cell
        void tire(const gcoords_t & pos, cell_base & clb);

        /// \brief Starts moving of a cell
        ///
        /// \param [in] pos Handle of the cell
        /// \param [in] clb Reference to the cell
        void start(const gcoords_t & pos, cell_base & clb);

        /// \brief Stops moving of a cell
        ///
        /// \param [in] pos Handle of the cell
        /// \param [in] clb Reference to the cell
        void halt(const gcoords_t & pos, cell_base & clb);

        /// \brief Applies changes of the process tab
        void apply();

        /// \brief Removes a cell from the process tab
        ///
        /// \param [in] pos Handle of the cell
        void remove(const gcoords_t & pos);

        /// \brief Returns the number of tabs on cells
        ///
        /// \return The number of tabs on cells
        size_t size() const;

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
