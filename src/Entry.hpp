//
// Created by nek on 2020-10-02.
//
#pragma once

#include "util.hpp"
#include <set>

class Entry {
public:
    [[nodiscard]] virtual double points() const = 0;

    [[nodiscard]] virtual std::string serialize() const = 0;

    [[nodiscard]] const std::string& get_name() const {
        return name;
    }

    void rename(const std::string& name) {
        Entry::name = name;
    }

    [[nodiscard]] id_type get_id() const {
        return id;
    }

    [[nodiscard]] double get_points() const {
        return cost;
    }

    void set_points(double cost) {
        Entry::cost = cost;
    }

    [[nodiscard]] const DateTime& get_created() const {
        return created;
    }

    [[nodiscard]] const std::string& get_descr() const {
        return descr;
    }

    void set_descr(const std::string& description) {
        descr = description;
    }

protected:

private:
    std::string name;
    id_type id;
    double cost = 0;
    DateTime created;
    std::string descr;
};

class Habit : public Entry {
public:
    Habit() = default;
    [[nodiscard]] double points() const override;
    [[nodiscard]] std::string serialize() const override;

    static constexpr char DELIM = '@';
private:
    //automatically sorted
    std::set<DateTime> check_ins;//when did we mark this habit as complete?
    bool archived = false;
    int reps = 0; //total
    int best_streak = 0;
    int total_completed = 0;
    int streak = 0; //current
    int reps_to_complete = 1; //number of repetitions to complete a habit
};

class Activity : public Entry {
public:
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
    static constexpr char DELIM = '#';
    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

private:
    bool completed = false;
    double difficulty_multiplier = 1;
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
