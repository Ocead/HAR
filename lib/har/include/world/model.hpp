//
// Created by Johannes on 19.06.2020.
//

#pragma once

#ifndef HAR_MODEL_HPP
#define HAR_MODEL_HPP

#include <har/types.hpp>

#include "world/world.hpp"

namespace har {

    class inner_simulation;

    namespace exception {
        class model_format_error : public exception {
        private:
            std::string _line;

        public:
            model_format_error(const std::string & source, const std::string & line);

            ~model_format_error() noexcept override;
        };
    }

    class model : public world {
    private:
        std::reference_wrapper<const inner_simulation> _sim;

        model_info _info;

    public:
        static model invalid;

        explicit model(const inner_simulation & sim);

        model(const model & ref);

        model(model && fref) noexcept;

        string_t & title();

        const string_t & title() const;

        string_t & author();

        const string_t & author() const;

        string_t & version();

        const string_t & version() const;

        string_t & description();

        const string_t & description() const;

        bool_t & editable();

        const bool_t & editable() const;

        model_info & info();

        const model_info & info() const;

        model & operator=(const model & ref);

        model & operator=(model && fref) noexcept;

        ~model();

        friend ostream & operator<<(ostream & os, const model & model);

        friend istream & operator>>(istream & is, model & model);

        friend istream & operator>>(istream & is, std::tuple<model &, bool_t &> model_ok);
    };

    ostream & operator<<(ostream & os, const model & model);

    istream & operator>>(istream & is, model & model);

    istream & operator>>(istream & is, std::tuple<model &, bool_t &> model_ok);
}

#endif //HAR_MODEL_HPP
