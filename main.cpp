#include "ExpertSystem.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Wrong number of arguments" << std::endl
                  << "Expected format: \"./expert-system file.txt\"" << std::endl;
        return (1);
    }
    try
    {
        ExpertSystem(std::string(argv[1]));
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    (void)argc;
    (void)argv;
    return (0);
}