#include "../includes/ExpertSystem.hpp"

// Constructor/Destructor

ExpertSystem::ExpertSystem(std::string fileName)
{
    // std::list<std::string> line;
    // std::string token;
    // for (unsigned long i = 0; i < fileName.size(); i++) {
    //     if (fileName[i] == ' ') {
    //         line.push_back(token);
    //         token = std::string();
    //         continue;
    //     }
    //     token.push_back(fileName[i]);
    // }
    // line.push_back(token);
    // auto rpn = getRPN(line);
    // for (auto token : rpn)
    //     std::cout << token << " ";
    // std::cout << std::endl;
    // exit(0);
    if (fileName.length() < 4)
        throw std::invalid_argument("File name is too short !");
    if (fileName[fileName.length() - 1] != 't' && fileName[fileName.length() - 2] != 'x' && fileName[fileName.length() - 3] != 't' && fileName[fileName.length() - 4] != '.')
        throw std::invalid_argument("File extension is not .txt");

    std::ifstream ifs(fileName);
    bool initialized = false;
    bool queryInitialized = false;
    std::string toInitialize;
    for (std::string line; std::getline(ifs, line);)
    {
        if (isCommentOrEmpty(line))
            continue;
        else if (isInitialFact(line))
        {
            if (initialized == true)
                throw std::invalid_argument("Multiple initial facts lines !");
            for (unsigned long i = 0; i < line.length(); i++)
            {
                if (isspace(line[i]))
                    continue;
                if (line[i] == '=')
                {
                    i++;
                    if (i >= line.length())
                        throw std::invalid_argument("Query line has to have atleast one character !");
                    while (i < line.length() && isupper(line[i]))
                        toInitialize += line[i++];
                }
                else
                    break;
            }
            initialized = true;
        }
        else if (isQuery(line))
        {
            if (initialized == false)
                throw std::invalid_argument("Query line is before the initial fact line !");
            if (queryInitialized == true)
                throw std::invalid_argument("Multiple query lines !");
            for (unsigned long i = 0; i < line.length(); i++)
            {
                if (isspace(line[i]))
                    continue;
                if (line[i] == '?')
                {
                    i++;
                    if (i >= line.length())
                        throw std::invalid_argument("Query line has to have atleast one character !");
                    while (i < line.length() && isupper(line[i]))
                        _queries.push_back(line[i++]);
                }
                else
                    break;
            }
            queryInitialized = true;
        }
        else // lines with facts
        {
            std::stringstream ss(line);
            std::string word;
            std::vector<std::string> tmp_vector;
            while (ss >> word)
            {
                if (word[0] == '#')
                    break;
                tmp_vector.push_back(word);
            }
            addLineToRules(tmp_vector);
        }
    }
    for (auto c : toInitialize)
        _facts[c] = true;

    // if (DEBUG)
    //     printDebug(toInitialize);

    expertLogic();
}

ExpertSystem::~ExpertSystem() {}

void ExpertSystem::expertLogic()
{
    for (auto query : _queries)
    {
        std::vector<std::vector<Token>> neighbours = createQueryNeighbours(_rules, query);
        for (auto neighbour : neighbours)
        {
            auto facts = _facts;
            int tmp = recursiveLogic(_rules, neighbour, facts);
            if (tmp == TRUE)
                _facts[query] = true;
        }
        for (auto fact : _facts)
        {
            std::cout << fact.first << " is " << fact.second << std::endl;
        }
        std::cout << std::endl;
    }
    /*
    while (queries)
        if (final condition (soit aucun voisin soit tous les rules explores))
            return true or false;
        pop le vector qu'on envoit de _rules
        return send to neighbours (fact of current line dans les results && pas explore)
    */
}

int ExpertSystem::recursiveLogic(std::vector<std::vector<Token>> rules, std::vector<Token> rule, std::map<char, int> facts)
{
    if (rules.empty() || rule.empty())
        return END_BRANCH;
    std::vector<Token> nextNeighbour;
    char checkCharacter = 0;
    rules = createNeighbours(rules, nextNeighbour, rule, checkCharacter);
    int branchResult;
    if (nextNeighbour.empty())
        return END_BRANCH;
    while (!nextNeighbour.empty())
    {
        branchResult = recursiveLogic(rules, nextNeighbour, facts);
        if (branchResult != END_BRANCH)
        {
            if (facts[checkCharacter] == true && branchResult == FALSE)
                std::invalid_argument("Contradiction found !");
            facts[checkCharacter] = branchResult;
        }
        rules = createNeighbours(rules, nextNeighbour, rule, checkCharacter);
        std::cout << std::endl;
    }
    return checkCondition(rule, facts);
}

int ExpertSystem::checkCondition(std::vector<Token> rule, std::map<char, int> &facts)
{
    (void)rule;
    (void)facts;
    std::cout << "In check condition" << std::endl;
    return TRUE;
};

std::vector<std::vector<Token>> ExpertSystem::createNeighbours(std::vector<std::vector<Token>> rules, std::vector<Token> &nextNeighbour, std::vector<Token> rule, char &character)
{
    nextNeighbour.clear();
    character = 0;
    std::string characters;
    for (unsigned long i = 0; rule[i].isImplicator == false; i++)
    {
        if (rule[i].isFact == true)
        {
            characters += rule[i].value;
            break;
        }
    }

    for (unsigned long j = 0; j < rules.size(); j++)
    {
        for (unsigned long k = rules[j].size() - 1; rules[j][k].isImplicator == false; k--)
        {
            if (rules[j][k].isResult == true)
            {
                if (characters.find(rules[j][k].value) != std::string::npos)
                {
                    character = rules[j][k].value[0];
                    if (rules[j][k].value[0] == '!')
                        character = rules[j][k].value[1];
                    nextNeighbour = rules[j];
                    rules.erase(rules.begin() + j);
                    break;
                }
            }
        }
    }
    return rules;
}

std::vector<std::vector<Token>> ExpertSystem::createQueryNeighbours(std::vector<std::vector<Token>> rules, char query)
{
    std::vector<std::vector<Token>> neighbours;
    for (auto rule : rules)
    {
        for (auto token : rule)
            if (token.isResult == true && (token.value[0] == query)) // || (token.value.size() == 2 && token.value[1] == query))) pas sur
            {
                neighbours.push_back(rule);
                break;
            }
    }
    // if (DEBUG)
    // {
    //     for (auto neighbour : neighbours)
    //         for (auto token : neighbour)
    //             std::cout << token << std::endl;
    //     std::cout << std::endl;
    // }
    return neighbours;
}

void ExpertSystem::printDebug(std::string toInitialize)
{
    for (auto it : _rules)
    {
        std::cout << std::endl;
        for (auto it2 : it)
            std::cout << it2 << std::endl;
    }
    std::cout << std::endl
              << std::endl
              << "_facts:" << std::endl;
    for (auto it : _facts)
        std::cout << it.first << " is " << it.second << " ";
    std::cout << std::endl
              << std::endl
              << "_queries:" << std::endl;
    for (auto it : _queries)
        std::cout << it;
    std::cout << std::endl
              << std::endl;
    std::cout << "Initial characters: " << std::endl
              << toInitialize << std::endl;
}

// Parsing methods

bool ExpertSystem::isCommentOrEmpty(std::string line) const
{
    for (unsigned long i = 0; i < line.length(); i++)
    {
        if (isalnum(line[i]))
            return (false);
        if (line[i] == '#')
            return (true);
    }
    return (true);
}

bool ExpertSystem::isQuery(std::string line) const
{
    for (unsigned long i = 0; i < line.length(); i++)
    {
        if (isspace(line[i]))
            continue;
        if (line[i] != '?')
            return (false);
        bool hasSpace = false;
        while (++i < line.length())
        {
            if (isupper(line[i]) && hasSpace == false)
                continue;
            if (isspace(line[i]))
            {
                hasSpace = true;
                continue;
            }
            if (line[i] == '#')
                return (true);
            else
                throw std::invalid_argument("Invalid query list line !");
        }
    }
    return (true);
}

bool ExpertSystem::isInitialFact(std::string line) const
{
    for (unsigned long i = 0; i < line.length(); i++)
    {
        if (isspace(line[i]))
            continue;
        if (line[i] != '=')
            return (false);
        bool hasSpace = false;
        while (++i < line.length())
        {
            if (isupper(line[i]) && hasSpace == false)
                continue;
            if (isspace(line[i]))
            {
                hasSpace = true;
                continue;
            }
            if (line[i] == '#')
                return (true);
            else
                throw std::invalid_argument("Invalid initial fact line !");
        }
    }
    return (true);
}

std::list<std::string> ExpertSystem::getRPN(std::list<std::string> line) {
    std::list<std::string> rpn;
    std::stack<std::string> operatorStack;
    for (auto token = line.begin(); token != line.end(); token++) {
        if (*token == "(") {
            std::list<std::string> subLine;
            for (int openCount = 0; *++token != ")" || openCount;) {
                if (*token == "(")
                    openCount++;
                else if (*token == ")")
                    openCount--;
                subLine.push_back(*token);
            }
            auto subRPN = getRPN(subLine);
            rpn.insert(rpn.end(), subRPN.begin(), subRPN.end());
            continue;
        }
        auto operatorPriority = _operatorPriorities.find(*token);
        if (operatorPriority != _operatorPriorities.end()) {
            while (!operatorStack.empty()) {
                auto lastOperatorPriority = _operatorPriorities.find(operatorStack.top());
                if (lastOperatorPriority->second <= operatorPriority->second) {
                    rpn.push_back(lastOperatorPriority->first);
                    operatorStack.pop();
                }
                else
                    break;
            }
            operatorStack.push(*token);
        }
        else
            rpn.push_back(*token);
    }
    while (!operatorStack.empty()) {
        rpn.push_back(operatorStack.top());
        operatorStack.pop();
    }
    return rpn;
}

void ExpertSystem::addLineToRules(std::vector<std::string> line)
{
    if (line.size() < 3)
        throw std::invalid_argument("A line has invalid format !");

    std::vector<Token> toToken;
    bool implicatorFound = false;

    for (auto it : line) // still need to check multiple Operators in a row, same for characters TODO
    {
        Token tmp;
        if (isupper(it[0]))
        {
            if (it.size() != 1)
                throw std::invalid_argument("A line has invalid format !");
            if (implicatorFound == false)
            {
                tmp.isFact = true;
                tmp.value = std::string(it);
                _facts[it[0]] = false;
            }
            else
            {
                tmp.isResult = true;
                tmp.value = std::string(it);
                _facts[it[0]] = false;
            }
        }
        else if (it[0] == '!')
        {
            if (it.size() != 2)
                throw std::invalid_argument("A line has invalid format !");
            if (isupper(it[1]))
            {
                if (implicatorFound == false)
                {
                    tmp.isFact = true;
                    tmp.isTrue = false;
                    tmp.value = std::string(it);
                    _facts[it[1]] = false;
                }
                else
                {
                    tmp.isResult = true;
                    tmp.isTrue = false;
                    tmp.value = std::string(it);
                    _facts[it[1]] = false;
                }
            }
        }
        else if (it.size() == 1 && (it[0] == '|' || it[0] == '+' || it[0] == '^'))
        {
            tmp.isOperator = true;
            tmp.value = std::string(it);
        }
        else if (it == "=>" || it == "<=>")
        {
            if (implicatorFound == true)
                throw std::invalid_argument("A line has two implicator !");
            tmp.isImplicator = true;
            tmp.value = std::string(it);
            implicatorFound = true;
        }
        else
        {
            throw std::invalid_argument("A line has invalid format !");
        }

        // add a function to check if there is atleast one fact/implicator/result TODO
        toToken.push_back(tmp);
    }
    checkLineValidity(toToken);
    _rules.push_back(toToken);
}

void ExpertSystem::checkLineValidity(std::vector<Token> toToken)
{
    bool hasFact = false;
    bool hasImplicator = false;
    bool hasResult = false;

    int lastWas = 0;

    if (toToken.size() < 3)
        throw std::invalid_argument("A line has invalid format !");
    if (toToken[0].isFact == false || toToken[toToken.size() - 1].isResult == false)
        throw std::invalid_argument("A line has to start with a fact and end with a result !");
    for (unsigned long i = 0; i < toToken.size(); i++)
    {
        Token it = toToken[i];
        // keep track ( and ) TODO
        if (hasFact == false && it.isFact == true)
            hasFact = true;
        else if (hasImplicator == false && it.isImplicator == true)
            hasImplicator = true;
        else if (hasResult == false && it.isResult == true)
            hasResult = true;
        if (it.isFact == true)
        {
            if (lastWas == 1)
                throw std::invalid_argument("Two fact in a row !");
            lastWas = 1;
        }
        else if (it.isOperator == true)
        {
            if (lastWas == 2)
                throw std::invalid_argument("Two operators in a row !");
            lastWas = 2;
        }
        else if (it.isResult == true)
        {
            if (lastWas == 3)
                throw std::invalid_argument("Two results in a row !");
            lastWas = 3;
        }
        else
        {
            lastWas = 0;
        }
    }
    if (hasFact != true || hasImplicator != true || hasResult != true)
        throw std::invalid_argument("Every line needs at least one fact, implicator, and argument !");
}