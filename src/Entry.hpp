//
// Created by nek on 2020-10-02.
//
#pragma once

namespace DB {



class Data {
public:
    static Data& get() {
        static Data inst;
        return inst;
    }
private:
    Data() = default;


};

} //namespace DB
