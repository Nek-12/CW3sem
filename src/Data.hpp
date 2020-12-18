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

    bool try_log_in(const std::string& username, const std::string& pass);

    void create_user(const std::string& l, const std::string& p);

    bool try_change_pass(const std::string& old_p, const std::string& new_p);

    bool check_pass(const std::string& p) { return hash(p) == hashed_pass; }

    bool logged_in() { return !(login.empty() || hashed_pass.empty()); }

    void log_out() noexcept;

    [[nodiscard]] const std::string& get_login() const {
        return login;
    }

    bool try_delete_user(const std::string& p);

    [[nodiscard]] int best_streak();

    [[nodiscard]] size_t completed_goals() const;

    [[nodiscard]] size_t incomplete_goals() const { return g.size() - completed_goals(); }

    //search everything for a selected string
    std::vector<Entry*> find(const std::string& s);

    std::vector<Entry*> find(const DateTime& dt, bool before);

    bool erase(const Habit& val);

    bool erase(const Goal& val);

    bool erase(const Activity& val);

    bool erase(const Entry& val);

    void sort_by_points() {
        auto comp = [](const Entry& l, const Entry& r) { return l.points() < r.points(); };
        sort_by(comp);
    }

    void sort_by_date() {
        auto comp = [](const Entry& l, const Entry& r) { return l.get_created() < r.get_created(); };
        sort_by(comp);
    }

    void sort_by_name() {
        auto comp = [](const Entry& l, const Entry& r) { return l.get_name() < r.get_name(); }; //NOLINT
        sort_by(comp);
    }

    void sort_by(bool(* bi_predicate)(const Entry&, const Entry&)) {
        h.sort(bi_predicate);
        g.sort(bi_predicate);
        a.sort(bi_predicate);
    }

    Journal<Habit> h; //NOLINT
    Journal<Goal> g; //NOLINT
    Journal<Activity> a; //NOLINT
private:
    std::string login;
    std::string hashed_pass;

    Data() = default;


};

