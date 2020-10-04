//
// Created by nek on 2020-10-02.
//
#pragma once
#include "util.hpp"

class Entry {
public:
    [[nodiscard]] virtual double points() const { return cost; };
    [[nodiscard]] virtual std::string serialize() const = 0;

    [[nodiscard]] id_type get_id() const {return id;}
    [[nodiscard]] std::string get_name() const {return name;}
    void rename(const std::string& new_name) {name = new_name;}
    void set_points(int raw_points) { cost = raw_points; }
    [[nodiscard]] const std::string& get_descr() const { return descr; }
    void set_descr(const std::string& description) { descr = description; }
protected:

private:
    std::string name;
    id_type id;
    double cost = 0;
    DateTime created;
    std::string descr;
};

class Habit: Entry {
public:
    [[nodiscard]] double points() const override;
    [[nodiscard]] std::string serialize() const override;
private:
    int reps = 0;
    int best_streak = 0;
    int total_completed = 0;
    int streak = 0 ; //current
    int reps_to_complete = 1; //number of repetitions to complete a habit
};

class Activity: Entry {
public:
    [[nodiscard]] double points() const override;
    [[nodiscard]] std::string serialize() const override;
private:
    DateTime time_elapsed;
    DateTime total_time;
    double benefit_multiplier = 1;
};

class Goal: Entry {
public:
    [[nodiscard]] double points() const override;
    [[nodiscard]] std::string serialize() const override;
private:
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
