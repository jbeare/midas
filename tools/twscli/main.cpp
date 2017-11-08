#include <iostream>
#include <string>

#include <Tws.h>

int main()
{
    auto tws = TwsInterface::MakeShared();

    while (1)
    {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "connect")
        {
            tws->Connect("127.0.0.1", 7497);
        }
        else if (line == "disconnect")
        {
            tws->Disconnect();
        }
        else if ((line == "exit") || (line == "quit"))
        {
            break;
        }
        else
        {
            std::cout << "Command not recognized." << std::endl;
        }
    }

    return 0;
}
