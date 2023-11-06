#pragma once
#include <list>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <iterator>

#ifndef DEBUG
#define DEBUG 0
#endif

#define BOTH_FALSE 100
#define BOTH_TRUE 200
#define TRUE_FALSE 300

// => is an implicator
// ! is (not) true
// D is the result

struct Token
{
    bool isOperator = false;   // + | ^ are operators
    bool isImplicator = false; // => <=> are implicators
    bool isNot = false;        // ! is (not) true
    char value;

    friend std::ostream &operator<<(std::ostream &os, const Token &rules)
    {
        os << "value: " << rules.value << " ";

        if (rules.isOperator)
            os << "is an operator ";
        if (rules.isImplicator)
            os << "is an implicator ";
        if (rules.isNot)
            os << " and is not ";
        else
            os << " and is false ";

        return os;
    }

    bool operator==(const Token &rhs) const { return this->value == rhs.value; }
};

class ExpertSystem
{
private:
    std::vector<std::vector<Token>> _rules;
    std::map<char, bool> _facts;
    std::string _queries;
    std::map<char, bool> _priorities = {std::pair<char, bool>('+', 1), std::pair<char, bool>('|', 2), std::pair<char, bool>('^', 3)};

public:
    // Constructors/Destructors
    ExpertSystem(std::string);
    ~ExpertSystem();

    // Parsing methods
    bool isCommentOrEmpty(std::string) const;
    bool isInitialFact(std::string) const;
    bool isQuery(std::string) const;

    //
    std::vector<Token> parseRule(std::string);
    std::vector<Token> makeRpnRule(std::vector<Token>);
    void printDebug(std::string);
    void expertLogic();
    bool findQueryValue(char);
    std::vector<Token> findQueryRule(char);
    bool implyRule(std::vector<Token>);
};