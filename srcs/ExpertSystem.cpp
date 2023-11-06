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
    std::set<std::string> toInitialize;
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
                    // if (i >= line.length())
                    //     throw std::invalid_argument("Query line has to have atleast one character !");
                    while (i < line.length() && isupper(line[i]))
                        toInitialize.insert(std::string(1, line[i++]));
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
        else // lines with _facts
            addRule(line);
    }

    // for (auto rule : _rules) {
    //     for (auto token : rule)
    //         std::cout << token.value << " ";
    //     std::cout << std::endl;
    // }

    for (auto c : toInitialize)
        _facts[c] = TRUE;

    // if (DEBUG)
    //     printDebug(toInitialize);

    expertLogic();
}

ExpertSystem::~ExpertSystem() {}

void ExpertSystem::expertLogic()
{
    std::vector<Token> initialRule;
    for (auto query : _queries)
    {
        Token token;
        token.isFact = true;
        token.value = query;
        initialRule.push_back(token);
    }
    // auto neighbours = createNeighbours(_rules, initialRule);
    // for (auto neighbour : neighbours)
    //     recursiveLogic(_rules, neighbour);
    std::map<std::string, int> backFacts = recursiveLogic(_facts, _rules, initialRule);
    // verifier si il y a des incoherences
    _facts = backFacts;
    for (auto fact : _facts)
    {
        if (DEBUG || _queries.find(fact.first) != std::string::npos)
        {
            std::cout << fact.first << " is ";
            if (fact.second == TRUE)
                std::cout << "true" << std::endl;
            else if (fact.second == FALSE)
                std::cout << "false" << std::endl;
            else
                std::cout << "undetermined" << std::endl;
        }
    }
    // if (DEBUG)
    //     for (auto rule : _rules)
    //         recursiveLogic(_facts, _rules, rule);
}

std::map<std::string, int> ExpertSystem::recursiveLogic(std::map<std::string, int> facts, std::vector<std::vector<Token>> rules, std::vector<Token> currentRule)
{
    // std::cout << "recursiveLogic start" << std::endl;
    if (rules.empty())
        return checkCondition(facts, currentRule);
    std::set<std::string> queries;
    for (auto token = currentRule.begin(); token != currentRule.end() && token->value != "=>"; token++)
        if (token->isFact)
            queries.insert(token->value);
    for (auto rule = rules.begin(); rule != rules.end(); rule++)
    {
        for (auto token = rule->begin(); token != rule->end(); token++)
        {
            if (token->isResult && queries.find(token->value) != queries.end())
            {
                auto backRule = *rule;
                rule = rules.erase(rule) - 1;
                std::map<std::string, int> backFacts = recursiveLogic(facts, rules, backRule);
                // verifier si il y a des incoherences
                facts = backFacts;
                break;
            }
        }
    }
    return checkCondition(facts, currentRule);
}

// std::vector<std::vector<Token>> ExpertSystem::createNeighbours(std::vector<std::vector<Token>> rules, std::vector<Token> currentRule)
// {
//     std::set<std::string> queries;
//     for (auto token = currentRule.begin(); token != currentRule.end() && !token->isImplicator; token++)
//         if (token->isFact)
//             queries.insert(token->value);
//     std::vector<std::vector<Token>> neighbours;
//     for (auto rule = rules.begin(); rule != rules.end(); rule++)
//     {
//         for (auto token = rule->begin(); token != rule->end(); token++)
//         {
//             if (token->isResult && queries.find(token->value) != queries.end())
//             {
//                 neighbours.push_back(*rule);
//                 break;
//             }
//         }
//     }
//     return neighbours;
// }

int ExpertSystem::implier(std::map<std::string, int> facts, std::vector<Token> rule)
{
    int result = -1;
    for (auto token = rule.begin(); token != rule.end(); token++)
    {
        if (token->isImplicator)
        {
            if (token->value == "=>")
                break;
            if (result == -1)
                result = facts.find((token - 1)->value) != facts.end() ? facts[(token - 1)->value] : FALSE;
            result = calculator(result, implier(facts, std::vector<Token>(token + 1, rule.end())), "+");
            if (result == FALSE)
            {
                throw std::invalid_argument("Contradiction !");
            }
            return result;
        }
        if (token->isOperator)
        {
            if (result == -1)
                result = facts.find((token - 2)->value) != facts.end() ? facts[(token - 2)->value] : FALSE;
            if ((token - 2)->isNot == true)
            {
                if (result == TRUE)
                    result = FALSE;
                else
                    result = TRUE;
            }
            int result2 = facts.find((token - 1)->value) != facts.end() ? facts[(token - 1)->value] : FALSE;
            if ((token - 1)->isNot == true)
            {
                if (result2 == TRUE)
                    result2 = FALSE;
                else
                    result2 = TRUE;
            }
            result = calculator(result, result2, token->value);
            token = rule.erase(token - 1, token + 1) - 1;
        }
    }
    if (result == -1)
    {
        return facts.find(rule[0].value) != facts.end() ? facts[rule[0].value] : FALSE;
    }
    return result;
}

int ExpertSystem::calculator(int first, int second, std::string op)
{
    if (DEBUG)
    {
    }
    if (op == "+")
    {
        if (first == TRUE && second == TRUE)
            return TRUE;
        return FALSE;
    }
    else if (op == "|")
    {
        if (first == TRUE || second == TRUE)
            return TRUE;
        return FALSE;
    }
    else
    {
        if ((first == TRUE && second == FALSE) || (first == FALSE && second == TRUE))
            return TRUE;
        return FALSE;
    }
}

std::map<std::string, int> ExpertSystem::checkCondition(std::map<std::string, int> facts, std::vector<Token> rule)
{
    // std::cout << "checkCondition start" << std::endl;
    (void)rule;
    if (DEBUG)
    {
        // std::cout << "rule looks like that in checkCondition:" << std::endl;
        for (auto it : rule)
            std::cout << it.value << " ";
        std::cout << std::endl;
    }
    bool resultOperation = false;
    resultOperation = implier(facts, rule);
    unsigned long i = 0;
    while (i < rule.size() && !rule[i].isImplicator)
        i++;
    if (i != rule.size() && rule[i].value == "<=>")
    {
        std::cout << "<=> is TRUE" << std::endl;
        return facts;
    }
    // if (DEBUG)
    // {
    //     std::cout << "char in _facts that are true: ";
    //     for (auto it : _facts)
    //     {
    //         if (it.second == TRUE)
    //             std::cout << it.first;
    //     }
    //     std::cout << std::endl;
    // }

    if (resultOperation == false)
        return facts;
    //     return FALSE;
    bool isUndetermined = false;
    // std::cout << "before result" << std::endl;
    if (resultOperation == true)
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
            if (rule[i].isResult == true && isUndetermined == true)
            {
                facts[rule[i].value] = UNDETERMINED;
                i++;
                continue;
            }
            if (rule[i].isResult == true && rule[i].isNot == false)
                facts[rule[i].value] = TRUE;
            else
            {
                if (facts[rule[i].value] == TRUE)
                {
                    std::cout << "exception?" << std::endl;
                    throw std::invalid_argument("Contradiction !");
                }
                else
                {
                    // std::cout << rule[i] << std::endl;
                    facts[rule[i].value] = FALSE;
                }
            }
            i++;
        };
    }
    if (DEBUG)
    {
        std::cout << "char in _facts that are true after set: ";
        for (auto it : facts)
        {
            if (it.second == TRUE)
                std::cout << it.first;
        }
        std::cout << std::endl;
    }
    // if (isUndetermined == true)
    //     return UNDETERMINED;
    // return TRUE;
    return facts;
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
    // bool biConditionalFound = false;
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
            // if (token->value == "<=>")
            //     biConditionalFound = true;
            // else
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
    // if (biConditionalFound)
    // {
    //     Token token;
    //     token.isOperator = true;
    //     token.value = "+";
    //     rule.push_back(token);
    // }

    return rpnRule;
}