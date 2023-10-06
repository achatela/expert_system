#include <vector>
#include <string>
#include <map>

// => is an implicator
// ! is (not) true
// D is the result
class ExpertSystem
{

    struct Rules
    {
        bool isFact;       // A B C... are facts
        bool isOperator;   // + | ^ are operators
        bool isImplicator; // => <=> are implicators
        bool isResult;     // characters after implicator = result
        bool isTrue;       // ! is (not) true

        std::string value;
    };

private:
    std::vector<Rules> _rules;
    std::map<char, int> _facts;
    std::vector<char> _queries;

public:
    ExpertSystem(std::string fileName);
    ~ExpertSystem();
};