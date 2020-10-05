//
// Created by nek on 2020-10-05.
//

#pragma once
#include "util.hpp"
#include <algorithm>
#include <numeric>

//TODO: sort() that can accept a lambda;

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

    template <typename ...Args>
    T& emplace_back(Args... args) {
        return v.emplace_back(std::forward(args)...);
    }

    auto push_back(const T& o) {
        return v.push_back(o);
    }

    void pop_back() {
        return v.pop_back();
    }

    T& operator[](size_t pos) {
        return v.at(pos);
    }

    const T& operator[](size_t pos) const {
        return v.at(pos);
    }

    void erase(size_t pos) {
        v.erase(std::advance(v.begin,pos));
    }

    std::vector<T> get_vec() const {
        return v;
    }

private:
    std::vector<T> v;
};


