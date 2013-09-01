#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>
 
#include "executor.h"
 
BOOST_AUTO_TEST_CASE(loop_executor)
{
    util::loop_executor exec;

    exec.add([]{ std::cout << "first " << "+" << " second" << std::endl;});
    BOOST_CHECK(exec.num_pending_closures() == 1);
    
    exec.add([]{ std::cout << "third " << "+" << " fourth" << std::endl;});
    BOOST_CHECK(exec.num_pending_closures() == 2);
    
    //exec.try_run_one_closure();
    //BOOST_CHECK(exec.num_pending_closures() == 1);
    
    exec.add([&]{ exec.make_loop_exit(); });
    BOOST_CHECK(exec.num_pending_closures() == 3);
    
    exec.add([]{ std::cout << "fifth " << "+" << " sixth" << std::endl;});
    BOOST_CHECK(exec.num_pending_closures() == 4);
    
    //exec.add([]{ throw std::exception();});
    
    exec.loop();
    
    std::cout << "-- " << exec.num_pending_closures() << std::endl;
    
    exec.try_run_one_closure();
    BOOST_CHECK(exec.num_pending_closures() == 0);
    exec.try_run_one_closure();
    BOOST_CHECK(exec.num_pending_closures() == 0);
 
}

BOOST_AUTO_TEST_CASE(thread_pool_executor)
{   
    util::thread_pool_executor pool(3);

    pool.add([]{ std::cout << "first " << "+" << " second" << std::endl;});
    pool.add([]{ std::cout << "first " << "+" << " second" << std::endl;});
    pool.add([]{ std::cout << "first " << "+" << " second" << std::endl;});
    
}   
