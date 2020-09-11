//
// Created by Johannes on 26.05.2020.
//

#include <iomanip>
#include <map>
#include <sstream>

#include <har/value.hpp>

using namespace har;

ostream & har::operator<<(ostream & os_, const color & c_) {
    os_ << '#' << std::hex
        << std::setfill(text('0')) << std::setw(sizeof(uint8_t) * 2) << static_cast<uint_t>(c_.r)
        << std::setfill(text('0')) << std::setw(sizeof(uint8_t) * 2) << static_cast<uint_t>(c_.g)
        << std::setfill(text('0')) << std::setw(sizeof(uint8_t) * 2) << static_cast<uint_t>(c_.b)
        << std::setfill(text('0')) << std::setw(sizeof(uint8_t) * 2) << static_cast<uint_t>(c_.a)
        << std::dec;
    return os_;
}

istream & har::operator>>(istream & is_, color & c_) {
    string_t c{ 3, text('\0') };
    size_t * e = nullptr;
    is_ >> c[0];
    if (c[0] != text('#'))
        return is_;
    is_.readsome(c.data(), 2);
    c_.r = std::stoul(c, e, 16);
    is_.readsome(c.data(), 2);
    c_.g = std::stoul(c, e, 16);
    is_.readsome(c.data(), 2);
    c_.b = std::stoul(c, e, 16);
    is_.readsome(c.data(), 2);
    c_.a = std::stoul(c, e, 16);
    return is_;
}

value & value::invalid() {
    static value val{ };
    return val;
}

of value::from_string(string_t & str) {
    static std::map<string_t, of> names = {
            { text("TYPE"),              of::TYPE },
            { text("NAME"),              of::NAME },
            { text("COLOR"),             of::COLOR },

            { text("POWERED_UP"),        of::POWERED_UP },
            { text("POWERED_DOWN"),      of::POWERED_DOWN },
            { text("POWERED_RIGHT"),     of::POWERED_RIGHT },
            { text("POWERED_LEFT"),      of::POWERED_LEFT },
            { text("POWERED_PIN"),       of::POWERED_PIN },
            { text("POWERING_DOWN"),     of::POWERING_DOWN },
            { text("POWERING_UP"),       of::POWERING_UP },
            { text("POWERING_LEFT"),     of::POWERING_LEFT },
            { text("POWERING_RIGHT"),    of::POWERING_RIGHT },
            { text("POWERING_PIN"),      of::POWERING_PIN },
            { text("POWERING_FIRST"),    of::POWERING_FIRST },

            { text("MOVED_UP"),          of::MOVED_UP },
            { text("MOVED_DOWN"),        of::MOVED_DOWN },
            { text("MOVED_RIGHT"),       of::MOVED_RIGHT },
            { text("MOVED_LEFT"),        of::MOVED_LEFT },
            { text("MOVING_UP"),         of::MOVING_UP },
            { text("MOVING_DOWN"),       of::MOVING_DOWN },
            { text("MOVING_RIGHT"),      of::MOVING_RIGHT },
            { text("MOVING_LEFT"),       of::MOVING_LEFT },
            { text("MOVING_FIRST"),      of::MOVING_FIRST },
            { text("MOVING_FROM"),       of::MOVING_FROM },
            { text("MOVING_TO"),         of::MOVING_TO },
            { text("MOTOR_DISTANCE"),    of::MOTOR_DISTANCE },

            { text("HAS_CARGO"),         of::HAS_CARGO },
            { text("HAS_ARTIFACTS"),     of::HAS_ARTIFACTS },

            { text("MAX_VOLTAGE"),       of::MAX_VOLTAGE },
            { text("HIGH_VOLTAGE"),      of::HIGH_VOLTAGE },
            { text("NEUTRAL_VOLTAGE"),   of::NEUTRAL_VOLTAGE },
            { text("LOW_VOLTAGE"),       of::LOW_VOLTAGE },
            { text("MIN_VOLTAGE"),       of::MIN_VOLTAGE },

            { text("ANALOG_VOLTAGE"),    of::ANALOG_VOLTAGE },
            { text("DIGITAL_VOLTAGE"),   of::DIGITAL_VOLTAGE },
            { text("PWM_VOLTAGE"),       of::PWM_VOLTAGE },
            { text("PWM_DUTY"),          of::PWM_DUTY },
            { text("PIN_MODE"),          of::PIN_MODE },

            { text("FIRING"),            of::FIRING },

            { text("FACING"),            of::FACING },

            { text("OPERATING_VOLTAGE"), of::OPERATING_VOLTAGE },

            { text("SPEED_FACTOR"),      of::SPEED_FACTOR },

            { text("LOGIC_POLARITY"),    of::LOGIC_POLARITY },

            { text("VALUE"),             of::VALUE },
            { text("MIN_VALUE"),         of::MIN_VALUE },
            { text("MAX_VALUE"),         of::MAX_VALUE },
            { text("VALUE_STEP"),        of::VALUE_STEP },

            { text("TIMER_STEP"),        of::TIMER_STEP },
            { text("TIMER_PAUSE"),       of::TIMER_PAUSE },

            { text("INT_HANDLER"),       of::INT_HANDLER },
            { text("INT_CONDITION"),     of::INT_CONDITION },

            { text("DESIGN"),            of::DESIGN }
    };
    auto it = names.find(str);
    if (it != names.end()) {
        return it->second;
    } else {
        return of::VOID;
    }
}

string_t value::to_string(of of) {
    static std::map<enum of, string_t> names = {
            { of::TYPE,              text("TYPE") },
            { of::NAME,              text("NAME") },
            { of::COLOR,             text("COLOR") },

            { of::POWERED_UP,        text("POWERED_UP") },
            { of::POWERED_DOWN,      text("POWERED_DOWN") },
            { of::POWERED_RIGHT,     text("POWERED_RIGHT") },
            { of::POWERED_LEFT,      text("POWERED_LEFT") },
            { of::POWERED_PIN,       text("POWERED_PIN") },
            { of::POWERING_DOWN,     text("POWERING_DOWN") },
            { of::POWERING_UP,       text("POWERING_UP") },
            { of::POWERING_LEFT,     text("POWERING_LEFT") },
            { of::POWERING_RIGHT,    text("POWERING_RIGHT") },
            { of::POWERING_PIN,      text("POWERING_PIN") },
            { of::POWERING_FIRST,    text("POWERING_FIRST") },

            { of::MOVED_UP,          text("MOVED_UP") },
            { of::MOVED_DOWN,        text("MOVED_DOWN") },
            { of::MOVED_RIGHT,       text("MOVED_RIGHT") },
            { of::MOVED_LEFT,        text("MOVED_LEFT") },
            { of::MOVING_UP,         text("MOVING_UP") },
            { of::MOVING_DOWN,       text("MOVING_DOWN") },
            { of::MOVING_RIGHT,      text("MOVING_RIGHT") },
            { of::MOVING_LEFT,       text("MOVING_LEFT") },
            { of::MOVING_FIRST,      text("MOVING_FIRST") },
            { of::MOVING_FROM,       text("MOVING_FROM") },
            { of::MOVING_TO,         text("MOVING_TO") },
            { of::MOTOR_DISTANCE,    text("MOTOR_DISTANCE") },

            { of::HAS_CARGO,         text("HAS_CARGO") },
            { of::HAS_ARTIFACTS,     text("HAS_ARTIFACTS") },

            { of::MAX_VOLTAGE,       text("MAX_VOLTAGE") },
            { of::HIGH_VOLTAGE,      text("HIGH_VOLTAGE") },
            { of::NEUTRAL_VOLTAGE,   text("NEUTRAL_VOLTAGE") },
            { of::LOW_VOLTAGE,       text("LOW_VOLTAGE") },
            { of::MIN_VOLTAGE,       text("MIN_VOLTAGE") },

            { of::ANALOG_VOLTAGE,    text("ANALOG_VOLTAGE") },
            { of::DIGITAL_VOLTAGE,   text("DIGITAL_VOLTAGE") },
            { of::PWM_VOLTAGE,       text("PWM_VOLTAGE") },
            { of::PWM_DUTY,          text("PWM_DUTY") },
            { of::PIN_MODE,          text("PIN_MODE") },

            { of::FIRING,            text("FIRING") },

            { of::FACING,            text("FACING") },

            { of::OPERATING_VOLTAGE, text("OPERATING_VOLTAGE") },

            { of::SPEED_FACTOR,      text("SPEED_FACTOR") },

            { of::LOGIC_POLARITY,    text("LOGIC_POLARITY") },

            { of::VALUE,             text("VALUE") },
            { of::MIN_VALUE,         text("MIN_VALUE") },
            { of::MAX_VALUE,         text("MAX_VALUE") },
            { of::VALUE_STEP,        text("VALUE_STEP") },

            { of::TIMER_STEP,        text("TIMER_STEP") },
            { of::TIMER_PAUSE,       text("TIMER_PAUSE") },

            { of::INT_HANDLER,       text("INT_HANDLER") },
            { of::INT_CONDITION,     text("INT_CONDITION") },

            { of::DESIGN,            text("DESIGN") }
    };
    auto it = names.find(of);
    if (it != names.end()) {
        return it->second;
    } else {
        return string_t();
    }
}

value value::get_standard(datatype t_) {
    switch (t_) {
        case datatype::VOID:
            return value(std::monostate());
        case datatype::BOOLEAN:
            return make_value<bool_t>(false);
        case datatype::INTEGER:
            return make_value<int_t>(0);
        case datatype::UNSIGNED:
            return make_value<uint_t>(0u);
        case datatype::DOUBLE:
            return make_value<double_t>(0.0f);
        case datatype::STRING:
            return make_value<string_t>(text(""));
        case datatype::C_COORDINATES:
            return make_value<ccoords_t>(ccoords_t(0., 0.));
        case datatype::D_COORDINATES:
            return make_value<dcoords_t>(dcoords_t(0, 0));
        case datatype::DIRECTION:
            return make_value<direction_t>(direction::NONE);
        case datatype::COLOR:
            return make_value<color_t>(color_t(0, 0, 0, 0));
        case datatype::SPECIAL:
            return make_value<special_t>();
        case datatype::CALLBACK:
            return make_value<callback_t>((void (*)(cell &)) nullptr);
        case datatype::HASH:
            return make_value<part_h>(0);
        default:
            return value();
    }
}

bool_t value::is_standard() {
    switch (datatype (index())) {
        case datatype::VOID:
            return true;
        case datatype::BOOLEAN:
            return !(get<bool_t>(*this));
        case datatype::INTEGER:
            return !(get<int_t>(*this));
        case datatype::UNSIGNED:
            return !(get<uint_t>(*this));
        case datatype::DOUBLE:
            return !(get<double_t>(*this));
        case datatype::STRING:
            return get<string_t>(*this).empty();
        case datatype::C_COORDINATES:
            return !(get<ccoords_t>(*this));
        case datatype::D_COORDINATES:
            return !(get<dcoords_t>(*this));
        case datatype::DIRECTION:
            return !(get<direction_t>(*this) != direction::NONE);
        case datatype::COLOR:
            return !(get<color_t>(*this));
        case datatype::SPECIAL:
        case datatype::CALLBACK:
        case datatype::HASH:
        default:
            return true;
    }
}

value::operator string_t() const {
    stringstream ss;
    ss << *this;
    return ss.str();
}

ostream & har::operator<<(ostream & os, const value & val) {
    switch (datatype(val.index())) {
        case datatype::VOID:
            break;
        case datatype::BOOLEAN:
            os << std::boolalpha << get<bool_t>(val) << std::noboolalpha;
            break;
        case datatype::INTEGER:
            os << get<int_t>(val);
            break;
        case datatype::UNSIGNED:
            os << get<uint_t>(val);
            break;
        case datatype::DOUBLE:
            os << get<double_t>(val);
            break;
        case datatype::STRING:
            os << std::quoted(get<string_t>(val), text('\"'), text('\\'));
            break;
        case datatype::C_COORDINATES:
            os << get<ccoords_t>(val);
            break;
        case datatype::D_COORDINATES:
            os << get<dcoords_t>(val);
            break;
        case datatype::DIRECTION:
            os << get<direction_t>(val);
            break;
        case datatype::COLOR:
            os << get<color_t>(val);
            break;
        case datatype::SPECIAL:
        case datatype::CALLBACK:
        case datatype::HASH:
        default:
            break;
    }
    return os;
}

string_t value::datatype_name(datatype type) {
    static std::map<datatype, string_t> names = {
            { datatype::VOID,          text("void") },
            { datatype::BOOLEAN,       text("har::bool_t") },
            { datatype::INTEGER,       text("har::int_t") },
            { datatype::UNSIGNED,      text("har::uint_t") },
            { datatype::DOUBLE,        text("har::double_t") },
            { datatype::STRING,        text("har::string_t") },
            { datatype::C_COORDINATES, text("har::ccoords_t") },
            { datatype::D_COORDINATES, text("har::dcoords_t") },
            { datatype::DIRECTION,     text("har::direction_t") },
            { datatype::COLOR,         text("har::color_t") },
            { datatype::SPECIAL,       text("har::special_t") },
            { datatype::CALLBACK,      text("har::callback_t") },
            { datatype::HASH,          text("har::part_h") }
    };

    return names.at(type);
}

datatype datatype_of(std::size_t val) {
    return datatype(val);
}

using namespace har::exception;

datatype_mismatch::datatype_mismatch(const std::string & source,
                                     value::datatype expected,
                                     value::datatype actual) : exception(source,
                                                                         "datatpye_mismatch in " + source +
                                                                         ":\n\texpected "),
                                                               _expected(expected),
                                                               _actual(actual) {
    stringstream css{ };
    css << value::datatype_name(expected) <<
        ", got " <<
        value::datatype_name(actual);
    auto str = css.str();
    _message += std::string(str.begin(), str.end());
}

const char * datatype_mismatch::what() const noexcept {
    return _message.c_str();
}

value::datatype datatype_mismatch::expected() const noexcept {
    return _expected;
}

value::datatype datatype_mismatch::actual() const noexcept {
    return _actual;
}

datatype_mismatch::~datatype_mismatch() noexcept = default;
