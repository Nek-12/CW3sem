//
// Created by nek on 2020-10-02.
//
#include "Entry.hpp"

double Habit::points() const {
    return 0;
}

std::string Habit::serialize() const {
    std::stringstream ss;
    char delim = DELIM;
    for(const auto& el : check_ins) {
        ss << delim << el;
        delim = ',';
    }
    delim = ';';
    ss << delim << archived << delim << reps << delim << best_streak << delim
    << total_completed << delim << streak << delim << reps_to_complete;
    return ss.str();
}


