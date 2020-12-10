#include "Data.hpp"
#include<filesystem>
#include "util.hpp"

bool Data::user_exists(const std::string& login) {
    return std::filesystem::exists(login + ".txt");
}

bool Data::try_log_in(const std::string& username, const std::string& pass) {
    if (!user_exists(username)) return false;
    if (logged_in()) throw std::logic_error("Already logged in");
    std::ifstream f(username + ".txt");
    std::string phash;
    std::getline(f, phash);
    Log() << "Read " << phash << " as a password for " << username;
    if (phash == hash(pass)) {
        this->login = username;
        this->hashed_pass = phash;
        return true;
    }
    return false;
}

//automatically logs the user in
void Data::create_user(const std::string& l, const std::string& p) {
    if (user_exists(l)) throw std::invalid_argument("such user already exists");
    std::ofstream f(l + ".txt");
    f << hash(p) << '\n';
    login = l;
    hashed_pass = hash(p);
}

void Data::save() {
    if (!logged_in()) throw std::runtime_error("Not logged in");
    std::ofstream f(login + ".txt");
    f << hashed_pass << '\n'
      << a.serialize()
      << h.serialize()
      << g.serialize();
}

void Data::load() try {
    if (!logged_in()) throw std::runtime_error("not logged in");
    std::ifstream f(login + ".txt");
    Log() << "Loading file " << login;
    std::string s;
    size_t line = 1;
    std::getline(f, s); //skip 1 line
    while (f) {
        ++line;
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
} catch (std::exception& e) {
    std::cerr << "Invalid data in the file " << login << ".txt\n" << e.what() << std::endl;
    throw e;
}

std::vector<Entry*> Data::find(const std::string& s) {
    std::vector<Entry*> ret;
    auto vph = h.find(s);
    auto vpa = g.find(s);
    auto vpg = a.find(s);
    ret.insert(
            ret.end(),
            std::make_move_iterator(vph.begin()),
            std::make_move_iterator(vph.end())
    );
    ret.insert(
            ret.end(),
            std::make_move_iterator(vpa.begin()),
            std::make_move_iterator(vpa.end())
    );
    ret.insert(
            ret.end(),
            std::make_move_iterator(vpg.begin()),
            std::make_move_iterator(vpg.end())
    );
    return ret;
}

std::vector<Entry*> Data::find(const DateTime& dt, bool before) {
    std::vector<Entry*> ret;
    auto vph = h.find(dt, before);
    auto vpa = g.find(dt, before);
    auto vpg = a.find(dt, before);
    ret.insert(
            ret.end(),
            std::make_move_iterator(vph.begin()),
            std::make_move_iterator(vph.end())
    );
    ret.insert(
            ret.end(),
            std::make_move_iterator(vpa.begin()),
            std::make_move_iterator(vpa.end())
    );
    ret.insert(
            ret.end(),
            std::make_move_iterator(vpg.begin()),
            std::make_move_iterator(vpg.end())
    );
    return ret;
}

bool Data::erase(const Habit& val) {
    return h.erase(val);
}

bool Data::erase(const Activity& val) {
    return a.erase(val);
}

bool Data::erase(const Goal& val) {
    return g.erase(val);
}

//the most general version that uses RTTI (slow)
bool Data::erase(const Entry& e) {
    if (typeid(e) == typeid(Habit&))
        return erase(static_cast<const Habit&>(e)); //NOLINT
    if (typeid(e) == typeid(Activity&))
        return erase(static_cast<const Activity&>(e)); //NOLINT
    if (typeid(e) == typeid(Goal&))
        return erase(static_cast<const Goal&>(e)); //NOLINT
    throw std::runtime_error("Unknown type encountered");
}

bool Data::try_delete_user(const std::string& p) try {
    if (!user_exists(login))
        return false;
    std::ifstream f(login + ".txt");
    std::string s;
    std::getline(f, s); //get a saved password
    if (hash(p) != s)
        return false;
    f.close();
    if (!std::filesystem::remove(login + ".txt"))
        return false;
    log_out();
    return true;
} catch (std::exception& e) {
    return false;
}

void Data::log_out() noexcept {
    login.clear();
    hashed_pass.clear();
    h.clear();
    g.clear();
    a.clear();
}

bool Data::try_change_pass(const std::string& old_p, const std::string& new_p) {
    if (hash(old_p) != hashed_pass)
        return false;
    hashed_pass = hash(new_p);
    return true;
}

int Data::best_streak() {
    int best = 0;
    for (const auto& el: h)
        if (best < el.get_streak())
            best = el.get_streak();
    return best;
}

size_t Data::completed_goals() const {
    size_t sum = 0;
    for (const auto& el: g)
        sum += el.is_completed();
    return sum;
}
