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
        if (isInitialFact(line))
        {
            if (initialized == true)
                throw std::invalid_argument("Multiple initial facts lines !");
            // logic to set the characters to true TODO
            initialized = true;
        }
        if (isQuery(line))
        {
            if (initialized == false)
                throw std::invalid_argument("Query line is before the initial fact line !");
            if (queryInitialized == true)
                throw std::invalid_argument("Multiple query lines !");
            // logic to set the query characters in the _queries TODO
            queryInitialized = true;
        }

        // std::cout << line << std::endl;
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