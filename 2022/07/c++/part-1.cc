#include "aoc/aoc.hpp"

class Directory {
public:
    explicit Directory(std::string name) : name_(std::move(name)) {}

    auto name() const -> std::string_view { return name_; }
    auto size() const -> size_t { return total_size_; }

    auto add_file(std::string name, size_t size) -> void {
        files_.insert_or_assign(std::move(name), size);
        increase_size(size);
    }

    auto add_directory(std::string name) -> std::reference_wrapper<Directory> {
        auto [it, inserted] = dirs_.try_emplace(name);
        AOC_ASSERT(inserted);
        it->second = std::make_unique<Directory>(std::move(name));
        it->second->parent_ = this;
        return *it->second;
    }

    auto get_dir(std::string const& name) const -> std::reference_wrapper<Directory> {
        if (name == ".."sv) {
            AOC_ASSERT(parent_);
            return *parent_;
        }
        return *dirs_.at(name);
    }

private:
    auto increase_size(size_t s) -> void {
        total_size_ += s;
        if (parent_) {
            parent_->increase_size(s);
        }
    }

    std::string name_;
    Directory* parent_ = nullptr;
    std::unordered_map<std::string, std::unique_ptr<Directory>> dirs_;
    std::unordered_map<std::string, size_t> files_;
    std::size_t total_size_ = 0;
};

template<std::ranges::input_range R>
auto solution(R&& input) {
    auto it = input.begin();
    auto const end = input.end();
    ++it;
    AOC_ASSERT(it != end);

    Directory root("/");
    std::vector<std::reference_wrapper<Directory>> all_directories;
    all_directories.push_back(root);
    std::reference_wrapper cwd = root;

    while (it != end) {
        auto const line = it->get();

        if (auto const match = aoc::regex_match(R"(\$ cd (.+))", line)) {
            cwd = cwd.get().get_dir((*match)[1].str());
            ++it;
        }
        if (auto const match = aoc::regex_match(R"(\$ ls)", line)) {
            ++it;
            while (it != end && it->get()[0] != '$') {
                if (auto const match = aoc::regex_match(R"(dir (.+))", it->get())) {
                    all_directories.push_back(cwd.get().add_directory((*match)[1].str()));
                }
                else if (auto const match = aoc::regex_match(R"((\d+) (.+))", it->get())) {
                    cwd.get().add_file((*match)[2].str(), aoc::parse<size_t>((*match)[1].str()));
                }
                else {
                    AOC_ASSERT(false);
                }
                ++it;
            }
        }
    }

    constexpr size_t threshold = 100000;
    size_t total = 0;
    for (Directory const& directory : all_directories) {
        if (directory.size() <= threshold) {
            total += directory.size();
        }
    }
    return total;
}

int main() {
    aoc::dbg(solution(aoc::input_range<aoc::line>()));
}
