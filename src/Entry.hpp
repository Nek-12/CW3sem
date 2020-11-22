#pragma once

#include "util.hpp"
#include <set>
#include <utility>

class Entry {
public:
    explicit Entry(id_type id, std::string name, double cost) : id(id), name(std::move(name)), cost(cost) {
        created = DateTime::get_current();
    }

    Entry(id_type id, std::string name, double cost, const DateTime& created) : id(id), name(std::move(
            name)), cost(cost), created(created) {}

    [[nodiscard]] virtual double points() const = 0;

    [[nodiscard]] virtual std::string serialize() const;

    [[nodiscard]] const std::string& get_name() const { return name; }

    void rename(const std::string& new_name) { name = new_name; }

    [[nodiscard]] id_type get_id() const { return id; }

    [[nodiscard]] double get_cost() const { return cost; }

    void set_cost(double points) { cost = points; }

    [[nodiscard]] const DateTime& get_created() const { return created; }

protected:
    static void validate_serialized_data(const std::string& s, char delimiter, int num_fields);

private:
    id_type id;
    std::string name;
    double cost = 0;
    DateTime created;
};


//TODO: the ability to undo a check-in

class Habit : public Entry {
public:
    Habit(id_type id, const std::string& name, double cost) : Entry(id, name, cost) {}

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

    [[nodiscard]] int get_best_streak() const { return best_streak; }

    [[nodiscard]] int get_streak() const { return streak; }

    void check_in();

    [[nodiscard]] auto get_check_ins() const { return check_ins; }

private:
    //automatically sorted
    std::set<DateTime> check_ins; //when did we mark this habit as complete?
    bool archived = false;
    int best_streak = 0;
    int streak = 0; //current
};

class Activity : public Entry {
public:
    Activity(id_type id, const std::string& name, double cost) : Entry(id, name, cost) {}

    Activity(id_type id, const std::string& name, double cost, const DateTime& created,
             std::set<DateTime> time_elapsed, const DateTime& total_time, double benefit_multiplier) :
            Entry(id, name, cost, created), time_elapsed(std::move(time_elapsed)),
            total_time(total_time), benefit_multiplier(benefit_multiplier) {}


    static Activity deserialize(const std::string& s);

    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

    void add_time(const DateTime& dt);

    [[nodiscard]] DateTime get_total_time() const { return total_time; }

    double get_benefit_multiplier() const { return benefit_multiplier; }

    void set_benefit_multiplier(double multiplier) { benefit_multiplier = multiplier; }

    static constexpr char DELIM = '$';
private:
    std::set<DateTime> time_elapsed; //date and how much time was spend on that day
    DateTime total_time; //total time spent on that activity
    double benefit_multiplier = 1; //by how much do we multiply the cost
};

class Goal : public Entry {
public:
    Goal(id_type id, const std::string& name, double cost) : Entry(id, name, cost) {}

    Goal(id_type id, const std::string& name, double cost, const DateTime& created, bool completed,
         const DateTime& est_length, const DateTime& deadline) :
            Entry(id, name, cost, created), completed(completed), est_length(est_length), deadline(deadline) {}

    static Goal deserialize(const std::string& s);

    [[nodiscard]] double points() const override;

    [[nodiscard]] std::string serialize() const override;

    [[nodiscard]] bool is_completed() const { return completed; }

    void toggle_completed() { completed = !completed; }

    [[nodiscard]] const DateTime& get_est_length() const { return est_length; }

    void set_est_length(const DateTime& est_length) { Goal::est_length = est_length; }

    [[nodiscard]] const DateTime& get_deadline() const { return deadline; }

    void set_deadline(const DateTime& deadline) { Goal::deadline = deadline; }

    DateTime time_left() { return deadline - DateTime::get_current(); }


    static constexpr char DELIM = '#';
private:
    bool completed = false;
    DateTime est_length;
    DateTime deadline;
};
