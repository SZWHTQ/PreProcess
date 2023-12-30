#ifndef EQUATION_OF_STATE_H
#define EQUATION_OF_STATE_H

#include <string>

class Equation_of_State {
protected:
    static size_t instanceCount;

public:
    size_t id;
    Equation_of_State(size_t _id)
        : id(_id) {};
    ~Equation_of_State()
    {
        --instanceCount;
    };
    static size_t getInstanceCount()
    {
        return instanceCount;
    }
    virtual std::string to_string(size_t count) = 0;
};

#endif