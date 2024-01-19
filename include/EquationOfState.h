#ifndef EQUATION_OF_STATE_H
#define EQUATION_OF_STATE_H

#include <string>

class EquationOfState {
   protected:
    static size_t instanceCount;

   public:
    size_t id;
    explicit EquationOfState(size_t _id) : id(_id){};
    ~EquationOfState() { --instanceCount; };
    static size_t getInstanceCount() { return instanceCount; }
    virtual std::string toString(size_t count) = 0;
};

#endif