#include "util.hpp"
#include "../lib/sha256.h"
#include <filesystem>
#include <random>
#include <cstddef>
#include <thread>
#include <chrono>

bool stob(const std::string& b) {
    if (lowercase(b) == "true")
        return true;
    if (lowercase(b) == "false")
        return false;
    throw std::invalid_argument(std::string("stob: invalid data ").append(b));
}

void wait(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::string hash(const std::string& s) {
    SHA256 sha256;
    return sha256(s);
}

std::vector<std::string> split(
        const std::string& s,
        const std::string& delims,
        bool empty_tokens_allowed) {
    std::vector<std::string> result;
    size_t current = 0;
    size_t next = -1;
    do {
        if (!empty_tokens_allowed) {
            next = s.find_first_not_of(delims, next + 1);
            if (next == std::string::npos) break;
            next -= 1;
        }
        current = next + 1;
        next = s.find_first_of(delims, current);
        result.push_back(s.substr(current, next - current));
    } while (next != std::string::npos);
    return result;
}

id_type gen_id() { // Static to not diminish randomness
    static std::default_random_engine e(
            std::random_device{}()); // Initialize a random engine from system
    // random device
    static std::uniform_int_distribution<id_type> rng(
            0, MAX_ID); // Generate numbers for IDs from 0 to a large system value
    return rng(e);  // use random engine
}

id_type stoid(const std::string& s) {
#ifndef __linux__
    return std::stoull(s);
#else // This function depends on platform. See header.h for details
    return std::stoul(s);
#endif
}


void ensure_file_exists(const std::string& f) {
    if (!std::filesystem::exists(f)) {
        Log() << "The file " << f << " does not exist! Creating a blank one...";
        std::ofstream file(f); // Create a new one
    }
}

// first is whether the check succeeded, second is error message
auto check_string(const std::string& s, CHECK mode)
-> std::pair<bool, std::string> {
    auto msgFalse = [&s](const std::string& msg) {
        return std::make_pair(false, "The value " + s + " is invalid: \n" +
                                     msg);
    };
    if (s.empty())
        return msgFalse("No data?");
    size_t cnt = 0;
    std::pair<bool, std::string> p;
    switch (mode) {
        case CHECK::PASS: // password
        case CHECK::WORD: // No spaces
            if (s.size() < 3 || s.size() > 75)
                return msgFalse("Too short/long for a word");
            for (const auto& ch : s)
                if (!(isalnum(ch) || ch == '.' || ch == '-' || ch == '_' ||
                      ch == '\'' || ch == '#'))
                    return msgFalse("Invalid characters");
            break;
        case CHECK::LINE: // spaces
            if (s.size() < 2)
                return msgFalse("Too short/long for a line");
            for (const auto& ch : s)
                if (!(isalnum(ch) || ispunct(ch) || ch == ' '))
                    return msgFalse("Invalid characters");
            break;
        case CHECK::ID: // integer
            if (s.size() > 15)
                return msgFalse("too long for a number");
            for (const auto& ch : s)
                if (!isdigit(ch))
                    return msgFalse("invalid characters in a number");
            break;
        case CHECK::DOUBLE: // float
            if (s.empty() || s.size() > 7)
                return msgFalse("too short/long for floating-point number");
            for (const auto& ch : s) {
                if (!isdigit(ch) && ch != '.')
                    return msgFalse("invalid characters in a number");
                if (ch == '.')
                    ++cnt;
                if (cnt > 1)
                    return msgFalse("not a number");
            }
            break;
        case CHECK::BOOL: // bool
            if (s.size() != 1 || (s[0] != '0' && s[0] != '1'))
                return msgFalse("not a boolean");
            break;
        case CHECK::TIME: { //time
            std::regex r{R"((\d{2})([-:.])(\d{2})([-:.])(\d{2}))"};
            std::smatch res;
            if (!std::regex_match(s, res, r))
                return msgFalse("Not a time or improperly formatted");
        }
            break;
        default:
            throw std::invalid_argument("Bad argument for checkString");
    }
    return std::make_pair(true, ""); //success
}

std::string lowercase(const std::string& s) {
    std::string ret;
    std::transform(s.begin(), s.end(), std::back_inserter(ret),
                   [](unsigned char c) { return std::tolower(c); });
    return ret;
}
