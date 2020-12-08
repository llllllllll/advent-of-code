#pragma once

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <cxxabi.h>

namespace aoc {
class demangle_error : public std::exception {
private:
    std::string m_msg;

public:
    demangle_error(const std::string& msg) : m_msg(msg) {}

    const char* what() const noexcept override {
        return m_msg.data();
    }
};

class invalid_mangled_name : public demangle_error {
public:
    invalid_mangled_name() : demangle_error("invalid mangled name") {}
};

/** Demangle the given string.
    @param cs The mangled symbol or type name.
    @return The demangled string.
 */
inline std::string demangle_string(const char* cs) {
    int status;
    char* demangled = ::abi::__cxa_demangle(cs, nullptr, nullptr, &status);

    switch (status) {
    case 0: {
        std::string out(demangled);
        std::free(demangled);
        return out;
    }
    case -1:
        throw demangle_error("memory error");
    case -2:
        throw invalid_mangled_name();
    case -3:
        throw demangle_error("invalid argument to ::abi::__cxa_demangle");
    default:
        throw demangle_error("unknown failure");
    }
}

/** Get the name for a given type. If the demangled name cannot be given, returns the
    mangled name.
    @tparam T The type to get the name of.
    @return The type's name.
 */
template<typename T>
std::string type_name() {
    const char* name = typeid(T).name();
    std::string out;
    try {
        out = demangle_string(name);
    }
    catch (const invalid_mangled_name&) {
        out = name;
    }

    if (std::is_lvalue_reference_v<T>) {
        out.push_back('&');
    }
    else if (std::is_rvalue_reference_v<T>) {
        out.insert(out.end(), 2, '&');
    }

    return out;
}
}  // namespace aoc
