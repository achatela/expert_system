#include "ExpertSystem.hpp"

// Constructor/Destructor

ExpertSystem::ExpertSystem(std::string fileName)
{
    if (fileName.length() < 4)
        throw std::invalid_argument("File name is too short !");
    if (fileName[fileName.length() - 1] != 't' && fileName[fileName.length() - 2] != 'x' && fileName[fileName.length() - 3] != 't' && fileName[fileName.length() - 4] != '.')
        throw std::invalid_argument("File extension is not .txt");

    std::ifstream ifs(fileName);
    for (std::string line; std::getline(ifs, line);)
    {
        if (isCommentOrEmpty(line))
        {
            std::cout << "comment" << std::endl;
            continue;
        }

        std::cout << line << std::endl;
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