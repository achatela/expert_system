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

#define FALSE 0
#define TRUE 1
#define UNDETERMINED 2
#define END_BRANCH 3

#define BOTH_FALSE 100
#define BOTH_TRUE 200
#define TRUE_FALSE 300

// => is an implicator
// ! is (not) true
// D is the result

struct Token
{
    bool isFact = false;       // A B C... are facts
    bool isOperator = false;   // + | ^ are operators
    bool isImplicator = false; // => <=> are implicators
    bool isResult = false;     // characters after implicator = result
    bool isNot = false;        // ! is (not) true
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
    std::map<char, int> _facts;
    std::vector<char> _queries;
    std::map<std::string, int> _priorities = {std::pair<std::string, int>("+", 1), std::pair<std::string, int>("|", 2), std::pair<std::string, int>("^", 3)};

public:
    // Constructors/Destructors
    ExpertSystem(std::string);
    ~ExpertSystem();

    // Parsing methods
    bool isCommentOrEmpty(std::string) const;
    bool isInitialFact(std::string) const;
    bool isQuery(std::string) const;

    //
    void addRule(std::string);
    std::vector<Token> makeRpnRule(std::vector<Token>);
    void printDebug(std::string);
    void expertLogic();
    int recursiveLogic(std::vector<std::vector<Token>>, std::vector<Token>, std::map<char, int> &);
    std::vector<std::vector<Token>> createQueryNeighbours(std::vector<std::vector<Token>>, char);
    std::vector<std::vector<Token>> createNeighbours(std::vector<std::vector<Token>> &, std::vector<Token>);
    // std::vector<std::vector<Token>> createNeighbours(std::vector<std::vector<Token>>, std::vector<Token> &, std::vector<Token>, char &, unsigned long &j);
    int checkCondition(std::vector<Token>, std::map<char, int> &);
    bool calculateOperation(std::string, std::string, std::string);
};