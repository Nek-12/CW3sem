#pragma once

#include "Journal.hpp"
#include "Entry.hpp"


class Data {
public:
    Data(const Data& d) = delete;

    Data(const Data&& d) = delete;

    Data operator=(const Data& rhs) = delete;

    static Data& get() {
        static Data inst;
        return inst;
    }

    void load();

    void save();

    [[nodiscard]] double total_points() const { return h.total_points() + g.total_points() + a.total_points(); }

    [[nodiscard]] double avg_points() const { return (h.avg_points() + g.avg_points() + a.avg_points()) / 3; }

    [[nodiscard]] double total_entries() const { return h.size() + g.size() + a.size(); }

    static bool user_exists(const std::string& login);

    bool attempt_login(const std::string& username, const std::string& pass);

    void create_user(const std::string& l, const std::string& p);

    void set_pass(const std::string& p) { hashed_pass = hash(p); }

    [[nodiscard]] const std::string& get_login() const {
        return login;
    }

    [[nodiscard]] int best_streak() {
        int best = 0;
        for (const auto& el: h)
            if (best < el.get_streak())
                best = el.get_streak();
        return best;
    }

    [[nodiscard]] size_t completed_goals() const {
        size_t sum = 0;
        for (const auto& el: g)
            sum += el.is_completed();
        return sum;
    }

    [[nodiscard]] size_t incomplete_goals() const {
        return g.size() - completed_goals();
    }

    //search everything for a selected string
    std::vector<Entry*> find(const std::string& s);

    std::vector<Entry*> find(const DateTime& dt, bool before);

    bool erase(const Habit& val);

    bool erase(const Goal& val);

    bool erase(const Activity& val);

    bool erase(const Entry& val);

    Journal<Habit> h; //NOLINT
    Journal<Goal> g; //NOLINT
    Journal<Activity> a; //NOLINT
private:
    std::string login;
    std::string hashed_pass;

    Data() = default;


};

