#include <Rhaast.h>

int main(  )
{
    auto rhaast = ( Rhaast* ) nullptr;

    /* print banner lol */
    std::cout << "\n"
                 "    ____  __                     __ \n"
                 "   / __ \\/ /_  ____ _____ ______/ /_\n"
                 "  / /_/ / __ \\/ __ `/ __ `/ ___/ __/\n"
                 " / _, _/ / / / /_/ / /_/ (__  ) /_  \n"
                 "/_/ |_/_/ /_/\\__,_/\\__,_/____/\\__/  \n"
                 "                                    " << std::endl;

    spdlog::set_pattern( "[%^%l%$] %v" );
    spdlog::info( "Rhaast [Version: 0.1]" );

    /* create a new rhaast client instance */
    rhaast = new Rhaast( "" );

    /* start routine */
    rhaast->Routine();

    /* free resource */
    delete rhaast;
}