//
// Created by admin on 2022/5/19.
//

#pragma once

#include "Type.h"
#include <iostream>
#include <list
#include <string>

namespace zfx {

    class Use {

    };
    class Value {
      public:

        explicit Value(Type *type, const std::string& name = "");

        ~Value() = default;

        std::list<Use> &get_use_list() {return use_list;}

        void add_use(Value *value);

        bool set_name(std::string name) {

        }

        std::string get_name() const;

        void remove_use(Value *val);

        virtual std::string print() = 0;
      private:
        std::string name;
        Type *type;
        std::list<Use> use_list;
    };
}