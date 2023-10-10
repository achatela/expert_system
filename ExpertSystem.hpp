#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <sstream>

// => is an implicator
// ! is (not) true
// D is the result
struct Rules
{
    bool isFact = false;       // A B C... are facts
    bool isOperator = false;   // + | ^ are operators
    bool isImplicator = false; // => <=> are implicators
    bool isResult = false;     // characters after implicator = result
    bool isTrue = true;        // ! is (not) true
    std::string value;

    friend std::ostream &operator<<(std::ostream &os, const Rules &rules)
    {
        os << "value: " << rules.value << " ";

        if (rules.isFact)
            os << "is a fact ";
        if (rules.isOperator)
            os << "is an operator ";
        if (rules.isImplicator)
            os << "is an implicator ";
        if (rules.isResult)
            os << "is a result ";
        if (rules.isTrue)
            os << " and is true ";
        else
            os << " and is false ";

        return os;
    }
};

class ExpertSystem
{
private:
    std::vector<std::vector<Rules>> _rules;
    std::map<char, int> _facts;
    std::vector<char> _queries;

public:
    // Constructors/Destructors
    ExpertSystem(std::string);
    ~ExpertSystem();

    // Parsing methods
    bool isCommentOrEmpty(std::string) const;
    bool isInitialFact(std::string) const;
    bool isQuery(std::string) const;

    //
    void addLineToRules(std::vector<std::string>);
    void checkLineValidity(std::vector<Rules>);
};