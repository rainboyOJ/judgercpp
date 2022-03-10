#include <iostream>
#include "./src/Server.hpp"

int main()
{

    Server myserver(9000,4);
    myserver.run();

    return 0;
}
