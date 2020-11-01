//
// Created by nek on 2020-10-12.
//

#include "Data.hpp"
#include<filesystem>
#include "util.hpp"

bool Data::user_exists(const std::string& login) {
    return std::filesystem::exists(login + ".txt");
}

bool Data::attempt_login(const std::string& login, const std::string& pass) {
    if (!user_exists(login)) return false;
    std::ifstream f(login + ".txt");
    std::string phash;
    std::getline(f, phash);
    if (phash == hash(pass)) {
        this->login = login;
        this->hashed_pass = hash(pass);
    }
    return false;
}

void Data::create_user(const std::string& l, const std::string& p) {
    if (user_exists(l)) throw std::invalid_argument("such user already exists");
    std::ofstream f(l + ".txt");
    f << hash(p) << '\n';
    login = l;
    hashed_pass = hash(p);
}

void Data::save() {
    std::ofstream f(login + ".txt");
    f << hashed_pass << '\n'
      << a.serialize()
      << h.serialize()
      << g.serialize();
}

void Data::load() {
    if (login.empty() || hashed_pass.empty()) throw std::runtime_error("not logged in");
    std::ifstream f(login + ".txt");
    Log() << "Loading file " << login;
    std::string s;
    std::getline(f, s); //skip 1 line
    while (f) {
        std::getline(f, s);
        Log() << "Got line: " << s;
        if (s.empty()) continue;
        switch (s[0]) {
            case Habit::DELIM:
                h.push_back(Habit::deserialize(s));
                break;
            case Activity::DELIM:
                a.push_back(Activity::deserialize(s));
                break;
            case Goal::DELIM:
                g.push_back(Goal::deserialize(s));
                break;
            default:
                throw std::runtime_error("invalid line/delimiter");
        }
    }
    Log() << "Finished loading: " << login << ".txt";
}
