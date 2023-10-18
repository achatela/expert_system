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
            addRule(line);
    }

    // for (auto rule : _rules) {
    //     for (auto token : rule)
    //         std::cout << token.value << " ";
    //     std::cout << std::endl;
    // }

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
            {
                std::cout << "\nQuery for : " << query << " is true" << std::endl
                          << std::endl;
                _facts[query] = true;
            }
            else if (tmp == FALSE)
            {
                if (_facts[query] == true)
                    std::invalid_argument("Contradiction !");
                std::cout << "\nQuery for : " << query << " is false" << std::endl
                          << std::endl;
            }
            else if (tmp == UNDETERMINED)
            {
                std::cout << "\nQuery for : " << query << " is undetermined" << std::endl;
            }
        }
        if (DEBUG)
        {
            for (auto fact : _facts)
            {
                std::cout << fact.first << " is " << fact.second << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

int ExpertSystem::recursiveLogic(std::vector<std::vector<Token>> rules, std::vector<Token> rule, std::map<char, int> facts)
{
    if (rules.empty() || rule.empty())
        return END_BRANCH;
    std::vector<Token> nextNeighbour;
    unsigned long j = 0;
    char checkCharacter = 0;
    rules = createNeighbours(rules, nextNeighbour, rule, checkCharacter, j);
    int branchResult;
    if (nextNeighbour.empty())
        return checkCondition(rule, facts);
    while (!nextNeighbour.empty())
    {
        branchResult = recursiveLogic(rules, nextNeighbour, facts);
        if (branchResult != END_BRANCH)
        {
            if (facts[checkCharacter] == true && branchResult == FALSE)
                std::invalid_argument("Contradiction found !");
            facts[checkCharacter] = branchResult;
        }
        rules = createNeighbours(rules, nextNeighbour, rule, checkCharacter, j);
        std::cout << std::endl;
    }
    return checkCondition(rule, facts);
}

int ExpertSystem::checkCondition(std::vector<Token> rule, std::map<char, int> &facts)
{
    (void)facts;
    (void)rule;
    if (DEBUG)
    {
        // std::cout << "rule looks like that :" << std::endl;
        for (auto it : rule)
            std::cout << it.value << " ";
        std::cout << std::endl;
    }
    bool resultOperation = false;
    unsigned long i = 0;
    std::list<std::string> ruleFacts;

    while (i < rule.size())
    {
        if (rule[i].isImplicator == true)
            break;
        if (rule[i].isOperator == false)
        {
            if (rule[i].isNot == true)
            {
                std::string toPush = (facts[rule[i].value[0]] == FALSE || facts[rule[i].value[0]] == UNDETERMINED) ? "true" : "false";
                ruleFacts.push_back(toPush);
            }
            else
            {
                std::string toPush = (facts[rule[i].value[0]] == FALSE || facts[rule[i].value[0]] == UNDETERMINED) ? "true" : "false";
                ruleFacts.push_back(toPush);
            }
        }
        else
            ruleFacts.push_back(rule[i].value);
        i++;
    }

    if (DEBUG)
    {
        std::cout << "char in facts that are true: ";
        for (auto it : facts)
        {
            if (it.second == TRUE)
                std::cout << it.first;
        }
        std::cout << std::endl;
    }

    if (ruleFacts.size() == 1)
    {
        resultOperation = false;
        if (ruleFacts.front() == "true")
            resultOperation = true;
        ruleFacts.pop_front();
    }

    while (!ruleFacts.empty())
    {
        unsigned long j = 0;
        auto it = ruleFacts.begin();
        while (*it == "false" || *it == "true")
        {
            it++;
            j++;
        }
        std::string oper = *it;
        it--;
        std::string second = *it;
        it--;
        std::string first = *it;
        resultOperation = calculateOperation(first, second, oper);
        for (int l = 3; l > 0; l--)
        {
            auto it2 = ruleFacts.begin();
            std::advance(it2, j - 2);
            ruleFacts.erase(it2);
        }
        if (ruleFacts.empty())
            break;
        auto it2 = ruleFacts.begin();
        std::advance(it2, j - 1);
        std::string toInsert = resultOperation == true ? "true" : "false";
        ruleFacts.insert(it2, toInsert);
    }
    bool isUndetermined = false;
    if (resultOperation == false)
        return FALSE;
    else
    {
        i++;
        unsigned long j = i;
        while (j < rule.size())
        {
            if (rule[j].isOperator == true)
            {
                if (rule[j].value == "|" || rule[j].value == "^")
                {
                    isUndetermined = true;
                    break;
                }
            }
            j++;
        }
        while (i < rule.size())
        {
            if (rule[i].isResult == true || rule[i].isNot == false)
            {
                facts[rule[i].value[0]] = UNDETERMINED;
                if (isUndetermined == false)
                    facts[rule[i].value[0]] = true;
            }
            else
            {
                if (facts[rule[i].value[0]] == true)
                {
                    std::cout << "exception?" << std::endl;
                    std::invalid_argument("Contradiction !");
                }
                facts[rule[i].value[0]] = UNDETERMINED;
                if (isUndetermined == false)
                    facts[rule[i].value[0]] = false;
            }
            i++;
        };
    }
    if (isUndetermined == true)
        return UNDETERMINED;
    return TRUE;
};

bool ExpertSystem::calculateOperation(std::string first, std::string second, std::string oper)
{
    int status = 0;
    if (first == "true" && second == "true")
        status = BOTH_TRUE;
    else if (first == "false" && second == "false")
        status = BOTH_FALSE;
    else
        status = TRUE_FALSE;

    if (oper == "+")
    {
        if (status == BOTH_TRUE)
            return true;
        return false;
    }
    else if (oper == "|")
    {
        if (status == BOTH_TRUE || status == TRUE_FALSE)
            return true;
        return false;
    }
    else if (oper == "^")
    {
        if (status == TRUE_FALSE)
            return true;
        return false;
    }
    if (DEBUG)
        std::cout << "NOT IN CONDITIONS ??" << std::endl
                  << std::endl;
    return false;
}

std::vector<std::vector<Token>> ExpertSystem::createNeighbours(std::vector<std::vector<Token>> rules, std::vector<Token> &nextNeighbour, std::vector<Token> rule, char &character, unsigned long &j)
{
    nextNeighbour.clear();
    character = 0;
    std::string characters;
    for (unsigned long i = 0; rule[i].isImplicator == false; i++)
    {
        if (rule[i].isFact == true)
        {
            characters += rule[i].value;
        }
    }
    for (; j < rules.size(); j++)
    {
        for (unsigned long k = rules[j].size() - 1; rules[j][k].isImplicator == false; k--)
        {
            if (rules[j][k].isResult == true)
            {
                if (characters.find(rules[j][k].value[0]) != std::string::npos)
                {
                    character = rules[j][k].value[0];
                    // if (rules[j][k].value[0] == '!')
                    //     character = rules[j][k].value[1];
                    nextNeighbour = rules[j];
                    rules.erase(rules.begin() + j);
                    goto fix;
                }
            }
        }
    }
fix:
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

void ExpertSystem::addRule(std::string line)
{
    std::vector<Token> rule;
    bool implicatorFound = false;
    int openCount = 0;
    for (unsigned long i = 0; i < line.size() && line[i] != '#'; i++)
    {
        if (line[i] == ' ')
            continue;
        if (line[i] == '(')
        {
            if (!rule.empty() && (rule.back().isFact || rule.back().isResult))
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.value = line[i];
            rule.push_back(token);
            openCount++;
        }
        else if (line[i] == ')')
        {
            if ((!rule.empty() && (!rule.back().isFact && !rule.back().isResult) && rule.back().value != ")") || !openCount)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.value = line[i];
            rule.push_back(token);
            openCount--;
        }
        else if (line[i] > 64 && line[i] < 91)
        {
            if (!rule.empty() && (rule.back().value == ")" || rule.back().isFact || rule.back().isResult))
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            if (implicatorFound == false)
                token.isFact = true;
            else
                token.isResult = true;
            token.value = line[i];
            rule.push_back(token);
        }
        else if (line[i] == '!' && line[i + 1] > 64 && line[i + 1] < 91)
        {
            if (!rule.empty() && (rule.back().value == ")" || rule.back().isFact || rule.back().isResult))
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            if (implicatorFound == false)
                token.isFact = true;
            else
                token.isResult = true;
            token.isNot = true;
            token.value = line[++i];
            rule.push_back(token);
        }
        else if (line[i] == '+' || line[i] == '|' || line[i] == '^')
        {
            if (rule.empty() || rule.back().isImplicator || rule.back().value == "(")
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isOperator = true;
            token.value = line[i];
            rule.push_back(token);
        }
        else if (line[i] == '=' && line[i + 1] == '>')
        {
            if (rule.empty() || rule.back().isImplicator || rule.back().value == "(" || implicatorFound)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isImplicator = true;
            token.value = "=>";
            rule.push_back(token);
            implicatorFound = true;
            i++;
        }
        else if (line[i] == '<' && line[i + 1] == '=' && line[i + 2] == '>')
        {
            if (rule.empty() || rule.back().isImplicator || rule.back().value == "(" || implicatorFound)
                throw std::invalid_argument("A line has invalid format !");
            Token token;
            token.isImplicator = true;
            token.value = "<=>";
            rule.push_back(token);
            implicatorFound = true;
            i += 2;
        }
        else
            throw std::invalid_argument("A line has invalid format !");
    }
    if (openCount || implicatorFound == false || (!rule.back().isResult && rule.back().value != ")"))
        throw std::invalid_argument("A line has invalid format !");
    _rules.push_back(makeRpnRule(rule));
}

std::vector<Token> ExpertSystem::makeRpnRule(std::vector<Token> rule)
{
    std::vector<Token> rpnRule;
    std::stack<Token> operatorStack;
    for (auto token = rule.begin(); token != rule.end(); token++)
    {
        if (token->isFact || token->isResult)
        {
            rpnRule.push_back(*token);
        }
        else if (token->isOperator)
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
        else
        {
            std::vector<Token> subRule;
            for (int openCount = 0; (++token)->value != ")" || openCount; subRule.push_back(*token))
            {
                if (token->value == "(")
                    openCount++;
                else if (token->value == ")")
                    openCount--;
            }
            std::vector<Token> subRpnRule = makeRpnRule(subRule);
            rpnRule.insert(rpnRule.end(), subRpnRule.begin(), subRpnRule.end());
        }
    }
    for (; !operatorStack.empty(); operatorStack.pop())
        rpnRule.push_back(operatorStack.top());
    return rpnRule;
}