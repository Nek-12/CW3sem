#pragma once

#include "util.hpp"
#include <algorithm>
#include <numeric>
#include <set>
#include "CONST.hpp"
#include "Entry.hpp"
//header-only class

template<std::derived_from<Entry> T> //use a concept to place constraints on values
class Journal {
public:
    void sort(bool(* bi_predicate)(const Entry&, const Entry&)) {
        std::sort(v.begin(), v.end(), bi_predicate);
    }

    void sort(std::function<bool(const T&, const T&)> bi_predicate) {
        std::sort(v.begin(), v.end(), bi_predicate);
    }

    [[nodiscard]] double total_points() const {
        return std::accumulate(v.begin(), v.end(), 0,
                               [](const double& lhs, const T& rhs) { return lhs + rhs.points(); });
    }

    [[nodiscard]] double avg_points() const {
        if (v.empty()) return 0;
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

    void clear() noexcept {
        return v.clear();
    }

    std::string serialize() {
        std::stringstream ss;
        for (const auto& e: v)
            ss << e.serialize() << '\n';
        return ss.str();
    }

    const T& top_points_value() {
        if (empty()) throw std::runtime_error("Top points on an empty journal"); //TODO Handle this better
        auto& ref = *std::max_element(v.begin(), v.end(),
                                      [](const T& a, const T& b) { return a.points() < b.points(); });
        Log() << "Called top points on " << ref.get_name();
        return ref;
    }


    [[nodiscard]] auto as_names() const -> std::vector<std::string_view> {
        std::vector<std::string_view> ret;
        Log() << "Returned " << v.size() << " names";
        for (const auto& el: v)
            ret.emplace_back(el.get_name());
        return ret;
    }

    [[nodiscard]] static auto as_names(std::vector<T*> vals) -> std::vector<std::string_view> {
        std::vector<std::string_view> ret;
        for (const auto& el: vals)
            ret.emplace_back(el->get_name());
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

    //returns all values filtered by the given unary predicate. If the predicate evaluates to true, the entry stays
    [[nodiscard]] std::vector<T*> filter(std::function<bool(const T&)> un_predicate) {
        std::vector<T*> ret;
        for (auto it = v.begin(); it != v.end(); ++it)
            if (un_predicate(*it))
                ret.push_back(&(*it));
        return ret;
    }

    [[nodiscard]] std::vector<T*> filter(bool archived) {
        return filter([archived](const T& val) { return val.is_archived() == archived; });
    }

    //returns pointers to all entries matching a pattern
    std::vector<T*> filter(const std::string& name) {
        return filter(
                [name](const T& val) { return lowercase(val.get_name()).find(lowercase(name)) != std::string::npos; });
    }

    //returns pointers to all entries before or after the selected date (inclusive)
    std::vector<T*> filter(const DateTime& created, bool before) {
        if (before)
            return filter([created](const T& val) { return val.get_created() <= created; });
        else
            return filter([created](const T& val) { return val.get_created() >= created; });
    }

    //returns an entry with a matching id, returns j.end() if no such entry exists;
    typename std::vector<T>::iterator obtain(id_type id) {
        return std::find(v.begin(), v.end(), [id](const T& el) { return el.get_id() == id; });
    }

    bool erase(const T& val) {
        auto it = std::find(v.begin(), v.end(), val);
        if (it == v.end())
            return false;
        Log() << "Erasing " << it->get_name();
        v.erase(it);
        return true;
    }

    [[nodiscard]] bool empty() const {
        return v.empty();
    }


private:
    std::vector<T> v;
};

