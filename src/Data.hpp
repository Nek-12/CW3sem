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

    Journal<Habit> h; //NOLINT
    Journal<Goal> g; //NOLINT
    Journal<Activity> a; //NOLINT
private:
    std::string login;
    std::string hashed_pass;

    Data() = default;


};

