#include <iostream>
#include "./src/Server.hpp"
#include "./src/utils.hpp"


int main()
{


    Server myserver(9000,4);
    myserver.run();

    return 0;
}
