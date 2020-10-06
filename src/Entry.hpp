//
// Created by nek on 2020-10-02.
//
#pragma once

#include "util.hpp"
#include <set>
#include <utility>

class Entry {
public:
    explicit Entry(id_type id) : id(id) {}

    Entry(id_type id, std::string name, double cost, const DateTime& created) : id(id), name(std::move(
            name)), cost(cost), created(created) {}

    [[nodiscard]] virtual double points() const = 0;

    [[nodiscard]] virtual std::string serialize() const;

    [[nodiscard]] const std::string& get_name() const { return name; }

    void rename(const std::string& name) { Entry::name = name; }

    [[nodiscard]] id_type get_id() const { return id; }

    [[nodiscard]] double get_cost() const { return cost; }

    void set_points(double points) { cost = points; }

    [[nodiscard]] const DateTime& get_created() const { return created; }

protected:

private:
    id_type id;
    std::string name;
    double cost = 0;
    DateTime created;
};

class Habit : public Entry {
public:
    explicit Habit(id_type id) : Entry(id) {}

    Habit(id_type id, std::string name, double cost, DateTime created, std::set<DateTime> check_ins, bool archived,
          int best_streak,
          int streak) : Entry(id, std::move(name), cost, created), check_ins(std::move(check_ins)), archived(archived),
                        best_streak(best_streak),
                        streak(streak) {}

    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

    static constexpr char DELIM = '@';

    static Habit deserialize(const std::string& s);

    void swap_archived() { archived = !archived; }

    [[nodiscard]] bool is_archived() const { return archived; }
    [[nodiscard]] int get_best_streak() const { return best_streak;}
    [[nodiscard]] int get_streak() const { return streak; }

private:
    //automatically sorted
    std::set<DateTime> check_ins; //when did we mark this habit as complete?
    bool archived = false;
    int best_streak = 0;
    int streak = 0; //current
};

class Activity : public Entry {
public:
    Activity(id_type id, const std::string& name, double cost, const DateTime& created,
             std::set<DateTime> time_elapsed, const DateTime& total_time, double benefit_multiplier) :
            Entry(id, name, cost, created), time_elapsed(std::move(time_elapsed)),
            total_time(total_time), benefit_multiplier(benefit_multiplier) {}

    explicit Activity(id_type id) : Entry(id) {}

    static Activity deserialize(const std::string& s);

    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

    static constexpr char DELIM = '$';
private:
    std::set<DateTime> time_elapsed; //date and how much time was spend on that day
    DateTime total_time; //total time spent on that activity
    double benefit_multiplier = 1; //by how much do we multiply the cost
};

class Goal : public Entry {
public:
    static Goal deserialize(const std::string& s);

    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

    static constexpr char DELIM = '#';
private:
    bool completed = false;
    DateTime est_length;
    DateTime deadline;
};

//////////////////////////////DATA////////////////////////////////

class Data {
public:
    static Data& get() {
        static Data inst;
        return inst;
    }

private:
    Data() = default;
};
