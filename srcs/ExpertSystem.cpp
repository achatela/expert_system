#include "../includes/ExpertSystem.hpp"

// Constructor/Destructor

ExpertSystem::ExpertSystem(std::string fileName)
{
    if (fileName.length() < 4)
        throw std::invalid_argument("File name is too short !");
    if (fileName[fileName.length() - 1] != 't' && fileName[fileName.length() - 2] != 'x' && fileName[fileName.length() - 3] != 't' && fileName[fileName.length() - 4] != '.')
        throw std::invalid_argument("File extension is not .txt");

    std::ifstream ifs(fileName);
    bool initialized = false;
    bool queryInitialized = false;
    for (std::string line; std::getline(ifs, line);)
    {
        if (isCommentOrEmpty(line))
            continue;
        else if (isInitialFact(line))
        {
            if (initialized == true)
                throw std::invalid_argument("Multiple initial _facts lines !");
            for (unsigned long i = 0; i < line.length(); i++)
            {
                if (isspace(line[i]))
                    continue;
                if (line[i] == '=')
                {
                    i++;
                    while (i < line.length() && isupper(line[i]))
                        _facts[line[i++]] = true;
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
        else
            _rules.push_back(makeRpnRule(parseRule(line)));
    }

    for (auto query : _queries)
        std::cout << query << " is " << findQueryValue(query) << std::endl;
    if (DEBUG)
        for (auto fact : _facts)
            if (_queries.find(fact.first) == std::string::npos)
                std::cout << fact.first << " is " << fact.second << std::endl;
}

ExpertSystem::~ExpertSystem() {}

bool ExpertSystem::findQueryValue(char query)
{
    return _facts.find(query) != _facts.end() ? _facts[query] : implyRule(findQueryRule(query));
}

std::vector<Token> ExpertSystem::findQueryRule(char query) {
    for (auto rule : _rules) {
        auto token = rule.begin();
        while (!token->isImplicator)
            token++;
        for (; token != rule.end(); token++)
            if (token->value == query)
                return rule;
    }
    return std::vector<Token>();
}

bool ExpertSystem::implyRule(std::vector<Token> rule)
{
    if (rule.empty())
        return false;
    if (DEBUG)
    {
        for (auto it : rule)
            std::cout << it.value << " ";
        std::cout << std::endl;
    }
    auto token = rule.begin();
    while (!token->isOperator && !token->isImplicator)
        token++;
    if (token->isImplicator) {
        if ((token - 1)->isNot)
            return !(_facts.find((token - 1)->value) != _facts.end() ? _facts[(token - 1)->value] : findQueryValue((token - 1)->value));
        return _facts.find((token - 1)->value) != _facts.end() ? _facts[(token - 1)->value] : findQueryValue((token - 1)->value);
    }
    bool first = _facts.find((token - 2)->value) != _facts.end() ? _facts[(token - 2)->value] : findQueryValue((token - 2)->value);
    if ((token - 2)->isNot)
        first = !first;
    for (; !token->isImplicator; token++) {
        if (token->isOperator) {
            bool second = _facts.find((token - 1)->value) != _facts.end() ? _facts[(token - 1)->value] : findQueryValue((token - 1)->value);
            if ((token - 1)->isNot)
                second = !second;
            if (token->value == '+')
                first = first && second;
            else if (token->value == '|')
                first = first || second;
            else
                first = first ^ second;
            token = rule.erase(token - 1, token + 1) - 1;
        }
    }
    if (DEBUG)
    {
        std::cout << "char in _facts that are true after set: ";
        for (auto it : _facts)
            if (it.second)
                std::cout << it.first;
        std::cout << std::endl;
    }
    return first;
};

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
        if (isalnum(line[i]) || line[i] == '=')
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

std::vector<Token> ExpertSystem::parseRule(std::string line) { 
    std::vector<Token> rule;
    bool implicatorFound = false;
    int openCount = 0;
    for (unsigned long i = 0; i < line.size() && line[i] != '#'; i++)
    {
        if (line[i] == ' ')
            continue;
        if (line[i] == '(')
        {
            if (!rule.empty() && !rule.back().isOperator && !rule.back().isImplicator)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.value = line[i];
            rule.push_back(token);
            openCount++;
        }
        else if (line[i] == ')')
        {
            if (!openCount || (!rule.empty() && !rule.back().isOperator && !rule.back().isImplicator))
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.value = line[i];
            rule.push_back(token);
            openCount--;
        }
        else if (line[i] > 64 && line[i] < 91)
        {
            if (!rule.empty() && !rule.back().isOperator && !rule.back().isImplicator)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.value = line[i];
            rule.push_back(token);
        }
        else if (line[i] == '!' && line[i + 1] > 64 && line[i + 1] < 91)
        {
            if (!rule.empty() && !rule.back().isOperator && !rule.back().isImplicator)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isNot = true;
            token.value = line[++i];
            rule.push_back(token);
        }
        else if (line[i] == '+' || line[i] == '|' || line[i] == '^')
        {
            if (rule.empty() || rule.back().isImplicator || rule.back().value == '(' || (implicatorFound && line[i] != '+'))
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isOperator = true;
            token.value = line[i];
            rule.push_back(token);
        }
        else if (line[i] == '=' && line[i + 1] == '>')
        {
            if (rule.empty() || openCount || implicatorFound || rule.back().isOperator)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isImplicator = true;
            rule.push_back(token);
            implicatorFound = true;
            i++;
        }
        else
            throw std::invalid_argument("A line has invalid format !");
    }
    if (openCount || implicatorFound == false || rule.back().isOperator || rule.back().isImplicator)
        throw std::invalid_argument("A line has invalid format !");
    return rule;
}

std::vector<Token> ExpertSystem::makeRpnRule(std::vector<Token> rule)
{
    std::vector<Token> rpnRule;
    std::stack<Token> operatorStack;
    for (auto token = rule.begin(); token != rule.end(); token++)
    {
        if (token->isOperator)
        {
            for (auto priority = _priorities.find(token->value); !operatorStack.empty() && _priorities.find(operatorStack.top().value)->second <= priority->second; operatorStack.pop())
                rpnRule.push_back(operatorStack.top());
            operatorStack.push(*token);
        }
        else if (token->isImplicator)
        {

            for (; !operatorStack.empty(); operatorStack.pop())
                rpnRule.push_back(operatorStack.top());
            rpnRule.push_back(*token);
        }
        else if (token->value == '(')
        {
            std::vector<Token> subRule;
            for (int openCount = 0; (++token)->value != ')' || openCount; subRule.push_back(*token))
            {
                if (token->value == '(')
                    openCount++;
                else if (token->value == ')')
                    openCount--;
            }
            std::vector<Token> subRpnRule = makeRpnRule(subRule);
            rpnRule.insert(rpnRule.end(), subRpnRule.begin(), subRpnRule.end());
        } else
            rpnRule.push_back(*token);
    }
    for (; !operatorStack.empty(); operatorStack.pop())
        rpnRule.push_back(operatorStack.top());
    return rpnRule;
}