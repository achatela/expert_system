#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <sstream>

#ifndef DEBUG
#define DEBUG 0
#endif

// => is an implicator
// ! is (not) true
// D is the result
struct Token
{
    bool isFact = false;       // A B C... are facts
    bool isOperator = false;   // + | ^ are operators
    bool isImplicator = false; // => <=> are implicators
    bool isResult = false;     // characters after implicator = result
    bool isTrue = true;        // ! is (not) true
    std::string value;

    friend std::ostream &operator<<(std::ostream &os, const Token &rules)
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
    std::vector<std::vector<Token>> _rules;
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
    void checkLineValidity(std::vector<Token>);
    void printDebug(std::string);
    void expertLogic();
    bool recursiveLogic(std::vector<std::vector<Token>>, std::vector<Token>, std::map<char, int> &);
    std::vector<std::vector<Token>> createQueryNeighbours(std::vector<std::vector<Token>>, char);
};