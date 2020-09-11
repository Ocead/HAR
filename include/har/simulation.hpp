//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_SIMULATION_HPP
#define HAR_SIMULATION_HPP

#include <deque>

#include <har/part.hpp>
#include <har/participant.hpp>

namespace har {

    class inner_simulation;

    ///
    class simulation {
    private:
        std::unique_ptr<inner_simulation> _isim; ///<

    public:

        /// \brief Default constructor
        explicit simulation(int argc = 0, char * argv[] = nullptr, char * envp[] = nullptr);

        explicit simulation(inner_simulation & isim);

        ///
        /// \param fref
        simulation(simulation && fref) noexcept;

        ///
        /// \param pt
        void include_part(const part & pt);

        ///
        /// \param id
        /// \return
        [[nodiscard]]
        const part & part_of(part_h id) const;

        void call_on_exit(const std::function<void()> & fun);

        void call_on_exit(std::function<void()> && fun);

        ///
        /// \param parti
        /// \return
        participant_h attach(participant & parti);

        ///
        /// \param id
        void datach(participant_h id);

        void commence();

        ///
        /// \param fref
        /// \return
        simulation & operator=(simulation && fref) noexcept;

        ///
        ~simulation();
    };

}

#endif //HAR_SIMULATION_HPP
