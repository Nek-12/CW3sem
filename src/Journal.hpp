#pragma once
#include "util.hpp"
#include <algorithm>
#include <numeric>
#include "CONST.hpp"

template<typename T>
class Journal {
public:
    void sort(bool(*ref)(const T&, const T&)) {
        std::sort(v.begin(),v.end(), ref);
    }

    [[nodiscard]] double total_points() const {
        return std::accumulate(v.begin(),v.end(),0,[](const double& lhs, const T& rhs){return lhs + rhs.points();});
    }

    [[nodiscard]] double avg_points() const {
        return total_points()/v.size();
    }

    [[nodiscard]] size_t size() const {
        return v.size();
    }

    template<typename ...Args>
    T& emplace_back(Args... args) {
        return v.emplace_back(args...);
    }

    void push_back(const T& o) {
        v.push_back(o);
    }

    void pop_back() {
        v.pop_back();
    }

    T& operator[](size_t pos) {
        return v.at(pos);
    }

    const T& operator[](size_t pos) const {
        return v.at(pos);
    }

    void erase(size_t pos) {
        v.erase(v.begin + pos);
    }

    std::string serialize() {
        std::stringstream ss;
        for (const auto& e: v)
            ss << e.serialize() << '\n';
        return ss.str();
    }


    auto as_names() -> std::vector<std::string_view> {
        std::vector<std::string_view> ret;
        for (const auto& el: v)
            ret.emplace_back(el.get_name());
        return ret;
    }

    auto begin() {
        return v.begin();
    }

    auto end() {
        return v.end();
    }

    auto begin() const {
        return v.begin();
    }

    auto end() const {
        return v.end();
    }

private:
    std::vector<T> v;
};


