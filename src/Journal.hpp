#pragma once

#include "util.hpp"
#include <algorithm>
#include <numeric>
#include <set>
#include "CONST.hpp"

template<typename T>
class Journal {
public:
    void sort(bool(* bi_predicate)(const T&, const T&)) {
        std::sort(v.begin(), v.end(), bi_predicate);
    }

    [[nodiscard]] double total_points() const {
        return std::accumulate(v.begin(), v.end(), 0,
                               [](const double& lhs, const T& rhs) { return lhs + rhs.points(); });
    }

    [[nodiscard]] double avg_points() const {
        return total_points() / v.size();
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

    void erase(typename std::vector<T>::iterator it) {
        v.erase(it);
    }

    std::string serialize() {
        std::stringstream ss;
        for (const auto& e: v)
            ss << e.serialize() << '\n';
        return ss.str();
    }


    [[nodiscard]] auto as_names() const -> std::vector<std::string_view> {
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

    //returns pointers to all entries matching a pattern
    std::vector<T*> find(const std::string& name) {
        std::vector<T*> ret;
        for (auto it = v.begin(); it != v.end(); ++it)
            if (it->get_name().find(name) != std::string::npos)
                ret.push_back(&(*it));
        return ret;
    }

    //returns pointers to all entries before or after the selected date (inclusive)
    std::vector<T*> find(const DateTime& created, bool before) {
        std::vector<T*> ret;
        if (before) {
            for (auto it = v.begin(); it != v.end(); ++it)
                if (it->get_created() <= created)
                    ret.push_back(&(*it));
        } else {
            for (auto it = v.begin(); it != v.end(); ++it)
                if (it->get_created() >= created)
                    ret.push_back(&(*it));
        }
        return ret;
    }

    //returns an entry with a matching id, returns j.end() if no such entry exists;
    typename std::vector<T>::iterator obtain(id_type id) {
        return std::find(v.begin(), v.end(), [id](const T& el) { return el.get_id() == id; });
    }

    bool erase(const T& val) {
        auto it = std::find(v.begin(), v.end(), val);
        if (it == v.end())
            return false;
        v.erase(it);
        return true;
    }

    [[nodiscard]] bool empty() const {
        return v.empty();
    }


private:
    std::vector<T> v;
};



