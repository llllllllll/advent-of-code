extern "C" {
#include <gnumake.h>
}  // extern "C"

#include <aoc/aoc.hpp>

extern "C" {
int plugin_is_GPL_compatible = 1;
}  // namespace "C"

namespace aoc::make_utils {
class mkstring {
public:
    explicit mkstring(size_t size) : ptr_{gmk_alloc(size)}, size_{size} {}
    mkstring(std::string_view view) : mkstring(view.size() + 1) {
        std::memcpy(ptr_, view.data(), view.size());
        ptr_[view.size()] = 0;
    }
    mkstring(const mkstring& cp) : mkstring(cp.size()) {
        std::memcpy(ptr_, cp.data(), cp.size() + 1);
    }
    mkstring(mkstring&& mv) : ptr_{mv.data()}, size_{mv.size()} {
        mv.ptr_ = nullptr;
        mv.size_ = 0;
    }

    ~mkstring() {
        if (ptr_) {
            gmk_free(ptr_);
        }
    }

    char* data() {
        return ptr_;
    }
    const char* data() const {
        return ptr_;
    }
    size_t size() const {
        return size_;
    }

    char& operator[](ptrdiff_t ix) {
        return ptr_[ix];
    }
    char operator[](ptrdiff_t ix) const {
        return ptr_[ix];
    }

    std::string_view view() const {
        return {ptr_, size_};
    }

    char* release() {
        const auto out = ptr_;
        ptr_ = nullptr;
        size_ = 0;
        return out;
    }

private:
    char* ptr_;
    size_t size_;
};

mkstring operator""_mk(const char* cs, size_t size) {
    return mkstring{std::string_view{cs, size}};
}

template<typename... Args>
mkstring error(Args&&... args) {
    std::cerr << "ERROR make-utils ";
    (std::cerr << ... << args) << '\n';
    return ""_mk;
}

mkstring to_mkstring(const char*, bool v) {
    if (v) {
        return "t"_mk;
    }
    return ""_mk;
}

template<std::integral I>
mkstring to_mkstring(const char* name, I v) {
    char buf[64];
    auto [end, ec] = std::to_chars(buf, buf + sizeof(buf), v);
    if (ec != std::errc{}) {
        return error(name, ": failed to convert int to string: ", v);
    }
    return mkstring{std::string_view(buf, end - buf)};
}

template<std::floating_point F>
mkstring to_mkstring(const char*, F v) {
    std::ostringstream oss;
    oss << v;
    return mkstring(oss.str());
}

template<typename T, auto op>
auto binop = +[](const char* name, unsigned int, char** argv) {
    try {
        return to_mkstring(name, op(parse<T>(argv[0]), parse<T>(argv[1]))).release();
    }
    catch (const bad_parse<T>& e) {
        return error(name, ": ", e.what()).release();
    }
};

template<typename T, auto op>
auto unop = +[](const char* name, unsigned int, char** argv) {
    try {
        return to_mkstring(name, op(parse<T>(argv[0]))).release();
    }
    catch (const bad_parse<T>& e) {
        return error(name, ": ", e.what()).release();
    }
};

char* range(const char* name, unsigned int argc, char** argv) {
    int64_t start;
    int64_t stop;
    int64_t step = 1;
    try {
        switch (argc) {
        case 1:
            start = 1;
            stop = aoc::parse<int64_t>(argv[0]);
            break;
        case 3:
            step = aoc::parse<int64_t>(argv[2]);
            [[fallthrough]];
        case 2:
            start = aoc::parse<int64_t>(argv[0]);
            stop = aoc::parse<int64_t>(argv[1]);
            break;
        default:
            return error(name, ": ", "expects between 1 and 3 arguments, got: ", argc)
                .release();
        }
    }
    catch (const aoc::bad_parse<int64_t>& e) {
        return error(name, ": ", e.what()).release();
    }
    std::ostringstream oss;
    for (int64_t i = start; i <= stop; i += step) {
        oss << i << ' ';
    }
    return mkstring(oss.str()).release();
}

char* list_init(const char*, unsigned int, char** argv) {
    std::string_view view{argv[0]};
    if (const auto ix = view.find_last_of(whitespace); ix != std::string_view::npos) {
        view.remove_suffix(view.size() - ix);
    }
    return mkstring(view).release();
}

char* list_tail(const char*, unsigned int, char** argv) {
    std::string_view view{argv[0]};
    if (const auto ix = view.find_first_of(whitespace); ix != std::string_view::npos) {
        view.remove_prefix(ix + 1);
    }
    return mkstring(view).release();
}

template<typename T>
char* sum(const char* name, unsigned int, char** argv) {
    T out = 0;
    try {
        for_split_first_of(std::string_view{argv[0]}, whitespace, [&](auto v) {
            if (!v.empty()) {
                out += parse<T>(v);
            }
        });
    }
    catch (const bad_parse<T>& e) {
        return error(name, ": ", e.what()).release();
    }
    return to_mkstring(name, out).release();
}

extern "C" int make_utils_gmk_setup() {
    gmk_add_function("int-add", binop<int64_t, std::plus<>{}>, 2, 2, 0);
    gmk_add_function("int-sub", binop<int64_t, std::minus<>{}>, 2, 2, 0);
    gmk_add_function("int-mul", binop<int64_t, std::multiplies<>{}>, 2, 2, 0);
    gmk_add_function("int-div", binop<int64_t, std::divides<>{}>, 2, 2, 0);
    gmk_add_function("int-mod", binop<int64_t, std::modulus<>{}>, 2, 2, 0);

    gmk_add_function("int-lt", binop<int64_t, std::less<>{}>, 2, 2, 0);
    gmk_add_function("int-le", binop<int64_t, std::less_equal<>{}>, 2, 2, 0);
    gmk_add_function("int-eq", binop<int64_t, std::equal_to<>{}>, 2, 2, 0);
    gmk_add_function("int-ne", binop<int64_t, std::not_equal_to<>{}>, 2, 2, 0);
    gmk_add_function("int-ge", binop<int64_t, std::greater_equal<>{}>, 2, 2, 0);
    gmk_add_function("int-gt", binop<int64_t, std::greater<>{}>, 2, 2, 0);

    gmk_add_function("int-and", binop<int64_t, std::bit_and<>{}>, 2, 2, 0);
    gmk_add_function("int-or", binop<int64_t, std::bit_or<>{}>, 2, 2, 0);
    gmk_add_function("int-xor", binop<int64_t, std::bit_xor<>{}>, 2, 2, 0);

    gmk_add_function("int-inc", unop<int64_t, [](auto a) { return ++a; }>, 1, 1, 0);
    gmk_add_function("int-dec", unop<int64_t, [](auto a) { return --a; }>, 1, 1, 0);
    gmk_add_function("int-neg", unop<int64_t, [](auto a) { return -a; }>, 1, 1, 0);
    gmk_add_function("int-inv", unop<int64_t, [](auto a) { return ~a; }>, 1, 1, 0);

    gmk_add_function("int-sum", sum<int64_t>, 1, 1, 0);

    gmk_add_function("float-add", binop<double, std::plus<>{}>, 2, 2, 0);
    gmk_add_function("float-sub", binop<double, std::minus<>{}>, 2, 2, 0);
    gmk_add_function("float-mul", binop<double, std::multiplies<>{}>, 2, 2, 0);
    gmk_add_function("float-div", binop<double, std::divides<>{}>, 2, 2, 0);

    gmk_add_function("float-lt", binop<double, std::less<>{}>, 2, 2, 0);
    gmk_add_function("float-le", binop<double, std::less_equal<>{}>, 2, 2, 0);
    gmk_add_function("float-eq", binop<double, std::equal_to<>{}>, 2, 2, 0);
    gmk_add_function("float-ne", binop<double, std::not_equal_to<>{}>, 2, 2, 0);
    gmk_add_function("float-ge", binop<double, std::greater_equal<>{}>, 2, 2, 0);
    gmk_add_function("float-gt", binop<double, std::greater<>{}>, 2, 2, 0);

    gmk_add_function("float-sum", sum<double>, 1, 1, 0);

    gmk_add_function("range", range, 1, 3, 0);
    return 1;
}
}  // namespace aoc::make_utils
