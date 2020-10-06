//
// Created by nek on 2020-10-02.
//
#include "Entry.hpp"
#include "CONST.hpp"

std::string Entry::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << id << delim << name << delim << cost << delim << created;
    return ss.str();
}

void Entry::validate_serialized_data(const std::string& s, char delimiter, int num_fields) {
    if (s.empty() || s.at(0) != delimiter || std::count(s.begin(),s.end(),';') != num_fields-1) //one ; less
        throw std::invalid_argument("attempted to deserialize " + s);
}

//////////////////HABIT/////////////////////

double Habit::points() const {
    //multiply base cost with percentage of completion
    // 1 + check-ins / (difference between days completed and days the habit exists)
    // coefficient will always be >1
    double a = ( 1 + check_ins.size() / (DateTime::get_current() - get_created()).to_d_approx());
    Log() << "Points multiplier of " << get_name() << ": " << a;

    return get_cost() * a + streak;
}

std::string Habit::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << DELIM << Entry::serialize();
    for (const auto& el : check_ins) {
        ss << delim << el;
        delim = ',';
    }
    delim = ';';
    ss << delim << archived << delim << best_streak << delim
       << streak << delim;
    Log() << "habit serialized: " << ss.str();
    return ss.str();
}

Habit Habit::deserialize(const std::string& s) {
    validate_serialized_data(s,DELIM,8);
    Log() << "habit to deserialize: " << s;
    auto v = split(s, ";", false);
    v[0].erase(0,1); //remove DELIM
    auto ci_svec = split(v[4], ",", false);
    std::set<DateTime> check_ins;
    for (const auto& el: ci_svec) //TODO: Find a suitable std:: algorithm
        check_ins.insert(DateTime::deserialize(el, DateTime::PAST));

    return Habit(stoid(v[0]), //id
                 v[1], //name
                 std::stod(v[2]), //cost
                 DateTime::deserialize(v[3],DateTime::PAST), //created
                  check_ins, //check_ins set
                 std::stoi(v[5]), //archived
                 std::stoi(v[6]), //best streak
                 std::stoi(v[7])); //streak
}

/////////////////////ACTIVITY/////////////////////////

/*
 * std::set<DateTime> time_elapsed; //date and how much time was spend on that day
    DateTime total_time; //total time spent on that activity
    double benefit_multiplier = 1; //by how much do we multiply the cost
 */

std::string Activity::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << DELIM << Entry::serialize();
    for (const auto& el : time_elapsed) {
        ss << delim << el;
        delim = ',';
    }
    delim = ';';
    ss << total_time << delim << benefit_multiplier;
    return ss.str();
}


Activity Activity::deserialize(const std::string& s) {
    validate_serialized_data(s,DELIM,7);
    auto v = split(s, ";", false);
    v[0].erase(0,1); //remove DELIM
    auto te_svec = split(v[4], ",", false);
    std::set<DateTime> time_elapsed;
    for (const auto& el: te_svec) //TODO: Find a suitable std:: algorithm
        time_elapsed.insert(DateTime::deserialize(el, DateTime::PAST));

    return Activity(stoid(v[0]), //id
                    v[1], //name
                    std::stod(v[2]), //cost
                    DateTime::deserialize(v[3], DateTime::PAST), //created
                    time_elapsed, //time_elapsed set
                    DateTime::deserialize(v[5],DateTime::ANY),  //total_time
                    std::stod(v[6])); //benefit_multiplier

}

double Activity::points() const {
    double a = ( 1 + (total_time.to_h_approx() * CONST::ACTIVITY_MULTIPLIER_PER_HOUR / (DateTime::get_current() - get_created()).to_h_approx() ));
    Log() << "Points multiplier of " << get_name() << ": " << a;
    return get_cost() * benefit_multiplier * a;
}

void Activity::add_time(const DateTime& dt) {
    time_elapsed.insert(dt);
    total_time+=dt;
}


////////////////GOAL//////////////////

/*
 *  bool completed = false;
    DateTime est_length;
    DateTime deadline;
 */

Goal Goal::deserialize(const std::string& s) {
    validate_serialized_data(s,DELIM,7);
    auto v = split(s,";",true);
    return Goal(stoid(v[0]), //id
                v[1], //name
                std::stod(v[2]), //cost
                DateTime::deserialize(v[3],DateTime::PAST), //created
                std::stoi(v[4]), //completed
                DateTime::deserialize(v[5], DateTime::ANY), //est_length
                DateTime::deserialize(v[6],DateTime::ANY)); //deadline
}

double Goal::points() const {
    return (get_cost() + est_length.to_h_approx())*completed;
}

std::string Goal::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << DELIM << Entry::serialize() << delim
    << completed << delim << est_length << delim << deadline;
    return ss.str();
}
