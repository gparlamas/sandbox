#include "executor.h"
#include <iostream>

int main(int argc, char* argv[])
{
    util::loop_executor exec;

    exec.add([]{ std::cout << "first " << "+" << " second" << std::endl;});
    std::cout <<"--" << exec.num_pending_closures() << std::endl;
    exec.try_run_one_closure();
    return 0;
}   
