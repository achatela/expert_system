#include "ExpertSystem.hpp"

// Constructor/Destructor

ExpertSystem::ExpertSystem(std::string fileName)
{
    if (fileName.length() < 4)
        throw std::invalid_argument("File name is too short !");
    if (fileName[fileName.length() - 1] != 't' && fileName[fileName.length() - 2] != 'x' && fileName[fileName.length() - 3] != 't' && fileName[fileName.length() - 4] != '.')
        throw std::invalid_argument("File extension is not .txt");

    std::ifstream ifs(fileName);
    std::vector<std::string> rules;
    bool initialized = false;
    bool queryInitialized = false;
    for (std::string line; std::getline(ifs, line);)
    {
        if (isCommentOrEmpty(line))
            continue;
        else if (isInitialFact(line))
        {
            if (initialized == true)
                throw std::invalid_argument("Multiple initial facts lines !");
            // logic to set the characters to true TODO
            initialized = true;
        }
        else if (isQuery(line))
        {
            if (initialized == false)
                throw std::invalid_argument("Query line is before the initial fact line !");
            if (queryInitialized == true)
                throw std::invalid_argument("Multiple query lines !");
            // logic to set the query characters in the _queries TODO
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
    for (auto it : _rules)
    {
        std::cout << std::endl;
        for (auto it2 : it)
            std::cout << it2 << std::endl;
    }
}

ExpertSystem::~ExpertSystem() {}

// Parsing methods

bool ExpertSystem::isCommentOrEmpty(std::string line) const
{
    for (int i = 0; i < line.length(); i++)
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
    for (int i = 0; i < line.length(); i++)
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
    for (int i = 0; i < line.length(); i++)
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

//

void ExpertSystem::addLineToRules(std::vector<std::string> line)
{
    if (line.size() < 3)
        throw std::invalid_argument("A line has invalid format !");

    std::vector<Rules> toRules;
    bool implicatorFound = false;

    for (auto it : line) // still need to check multiple Operators in a row, same for characters TODO
    {
        Rules tmp;
        if (isupper(it[0]))
        {
            if (it.size() != 1)
                throw std::invalid_argument("A line has invalid format !");
            if (implicatorFound == false)
            {
                tmp.isFact = true;
                tmp.value = std::string(it);
            }
            else
            {
                tmp.isResult = true;
                tmp.value = std::string(it);
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
                }
                else
                {
                    tmp.isResult = true;
                    tmp.isTrue = false;
                    tmp.value = std::string(it);
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
        toRules.push_back(tmp);
    }
    checkLineValidity(toRules);
    _rules.push_back(toRules);
}

void ExpertSystem::checkLineValidity(std::vector<Rules> toRules)
{
    bool hasFact = false;
    bool hasImplicator = false;
    bool hasResult = false;

    int lastWas = 0;

    if (toRules.size() < 3)
        throw std::invalid_argument("A line has invalid format !");
    if (toRules[0].isFact == false || toRules[toRules.size() - 1].isResult == false)
        throw std::invalid_argument("A line has to start with a fact and end with a result !");
    for (int i = 0; i < toRules.size(); i++)
    {
        Rules it = toRules[i];
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
    // if (hasFact != true || hasImplicator != true || hasResult != true)
    //     throw std::invalid_argument("Every line needs at least one fact, implicator, and argument !");
}