#include "Entry.hpp"
#include "CONST.hpp"
#include "UI.hpp"

std::string Entry::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << id << delim << name << delim << cost << delim << created << delim << std::boolalpha << archived;
    return ss.str();
}

void Entry::validate_serialized_data(const std::string& s, char delimiter, int num_fields) {
    if (s.empty() || s.at(0) != delimiter || std::count(s.begin(), s.end(), ';') != num_fields - 1) //one ; less
        throw std::invalid_argument("attempted to deserialize " + s);
}

bool Entry::operator==(const Entry& rhs) const {
    return id == rhs.id;
}

bool Entry::operator!=(const Entry& rhs) const {
    return !(rhs == *this);
}

std::string Entry::summary() const {
    std::stringstream ss;
    Log() << "Name: " << name << "id: " << id;
    ss << color::bold_blue
       << "Title: " << name << '\n'
       << "Cost: " << cost << " points\n"
       << (archived ? "Is " : "Isn't ") << " archived\n"
       << "Created at " << created.to_printable(true) << color::reset << '\n';

    return ss.str();
}

//////////////////HABIT/////////////////////

double Habit::points() const {
    //multiply base cost with percentage of completion
    // 1 + check-ins / (difference between days completed and days the habit exists)
    // coefficient will always be >1
    double a = (1 + check_ins.size() / (DateTime::get_current() - get_created()).to_d_approx());
    Log() << "Points multiplier of " << get_name() << ": " << a;
    return (get_cost() * a + streak) * !is_archived();
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
    if (check_ins.empty())
        ss << delim; //empty value
    ss << delim << best_streak << delim << streak;
    Log() << "habit serialized: " << ss.str();
    return ss.str();
}

Habit Habit::deserialize(const std::string& s) {
    validate_serialized_data(s, DELIM, 8);
    Log() << "habit to deserialize: " << s;
    auto v = split(s, ";", true); //even if there are no check-ins, it's valid
    v[0].erase(0, 1); //remove DELIM
    auto ci_svec = split(v[5], ",", false);
    std::set<DateTime> check_ins;
    for (const auto& el: ci_svec) //TODO: Find a suitable std:: algorithm
        check_ins.insert(DateTime::deserialize(el, DateTime::PAST));

    return Habit(stoid(v[0]), //id
                 v[1], //name
                 std::stod(v[2]), //cost
                 DateTime::deserialize(v[3], DateTime::PAST), //created
                 check_ins, //check_ins set
                 stob(v[4]), //archived
                 std::stoi(v[6]), //best streak
                 std::stoi(v[7])); //streak
}

bool Habit::check_in() {
    if (is_archived()) return false;
    auto cur = DateTime::get_current();
    if (!check_ins.empty()) { //if it's not the first
        const DateTime& last = *check_ins.crbegin(); //get the last element
        //if last check in's DAY (2) is one less than today's
        if (cur[0] == last[0] &&
            cur[1] == last[1] &&
            cur[2] == last[2] + 1)
            ++streak;
        else streak = 0; //if not, reset the streak :(
    } else ++streak; //if first, just increment
    check_ins.insert(cur);
    if (best_streak < streak) best_streak = streak;
    return true;
}

std::string Habit::summary() const {
    std::stringstream ss;
    ss << Entry::summary() << color::bold_blue
       << "Best streak: " << best_streak << " days\n"
       << "Current streak: " << streak << " days\n"
       << "Check ins: ";
    std::string delim = "\n";
    for (const auto& el: check_ins) {
        ss << delim << el;
        delim = ", \n";
    }
    ss << color::reset << '\n';
    return ss.str();
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
    if (time_elapsed.empty())
        ss << delim; //empty is valid
    ss << delim << total_time << delim << benefit_multiplier;
    return ss.str();
}


Activity Activity::deserialize(const std::string& s) {
    validate_serialized_data(s, DELIM, 8);
    Log() << "activity to deserialize: " << s;
    auto v = split(s, ";", true);
    v[0].erase(0, 1); //remove DELIM
    auto te_svec = split(v[5], ",", false);
    std::set<DateTime> time_elapsed;
    for (const auto& el: te_svec) //TODO: Find a suitable std:: algorithm
        time_elapsed.insert(DateTime::deserialize(el, DateTime::PAST));

    return Activity(stoid(v[0]), //id
                    v[1], //name
                    std::stod(v[2]), //cost
                    DateTime::deserialize(v[3], DateTime::PAST), //created
                    time_elapsed, //time_elapsed set
                    DateTime::deserialize(v[6], DateTime::ANY),  //total_time
                    std::stod(v[7]), //benefit_multiplier
                    stob(v[4]));
}

double Activity::points() const {
    double a = (1 + (total_time.to_h_approx() * CONST::ACTIVITY_MULTIPLIER_PER_HOUR /
                     (DateTime::get_current() - get_created()).to_h_approx()));
    Log() << "Points multiplier of " << get_name() << ": " << a;
    return get_cost() * benefit_multiplier * a * !is_archived();
}

void Activity::add_time(const DateTime& dt) {
    time_elapsed.insert(dt);
    total_time += dt;
    Log() << "Added " << dt << " to " << this->get_name();
}

std::string Activity::summary() const {
    std::stringstream ss;
    ss << Entry::summary() << color::bold_blue
       << "Point multiplier: x" << benefit_multiplier << "\n"
       << "Total time spent on the activity: " << total_time.to_duration_printable() << "\n"
       << "Elapsed time records: ";
    std::string delim = "\n";
    for (const auto& el: time_elapsed) {
        ss << delim << el;
        delim = ", \n";
    }
    ss << color::reset << '\n';
    return ss.str();
}


////////////////GOAL//////////////////

/*
    DateTime est_length;
    DateTime deadline;
 */

Goal Goal::deserialize(const std::string& s) {
    validate_serialized_data(s, DELIM, 7);
    Log() << "goal to deserialize: " << s;
    auto v = split(s, ";", true);
    v[0].erase(0, 1);
    return Goal(stoid(v[0]), //id
                v[1], //name
                std::stod(v[2]), //cost
                DateTime::deserialize(v[3], DateTime::PAST), //created
                stob(v[4]), //completed
                DateTime::deserialize(v[5], DateTime::ANY), //est_length
                DateTime::deserialize(v[6], DateTime::ANY)); //deadline
}

double Goal::points() const {
    return (get_cost() + est_length.to_h_approx()) * !is_completed();
}

std::string Goal::serialize() const {
    std::stringstream ss;
    char delim = ';';
    ss << DELIM << Entry::serialize() << delim << est_length << delim << deadline;
    return ss.str();
}

std::string Goal::summary() const {
    std::stringstream ss;
    ss << Entry::summary() << color::bold_blue
       << "Estimated length: " << est_length.to_duration_printable() << "\n";
    if (!deadline.incomplete() && deadline < DateTime::get_current() && !is_completed()) {
        ss << color::red << "The task is overdue! It was due on: \n"
           << deadline.to_printable(true);
    } else {
        ss << "Due on: " << deadline.to_printable(true);
    }
    ss << color::reset << '\n';
    return ss.str();
}
