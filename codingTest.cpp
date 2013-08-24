#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <vector>
#include <string>

/*

   A coding interview I had in the past.

Given a number and the function below, you should compute the following data about the generated series:

1. Maximum numberin the series

2. Zero based index in the series of the maximum number

3. Maximum difference between successive numbers in the series

4. Minimum difference between successive numbers in the series5. Total of all of the numbersin the series

6. List the numbers in the series that are wholly divisible by 5 in the orderthatthey appear

 
*/

inline long long generator(int value)
{
    return value % 2 == 0 ? value / 2 : 3 * value + 1;
}

struct printDivisibleByFive : std::unary_function<void, long long>
{
    void operator()(long long value)
    {
        if (value % 5 == 0)
        {
            std::cout << value << ' ';
        }
    }
};

template <typename T>
struct difference : std::binary_function<T, T, T>
{
    //by defaut, std::adjacent_difference without a predicate computes the result as
    //dest[0] = source[0];
    //dest[1] = source[1] - source[0] etc
    //in the example for seq {10, 5, 16, 8, 4, 2, 1} your output is 8, -11 so we need to calc the difference as source[0] - source[1]
    T operator()(T current, T previous) { return previous - current; } 
};

//custom loop to calculate, in place, min/max adjacent difference.
template <typename InputIterator, typename Result>
std::pair<Result, Result> custom_adjacent_difference(InputIterator first, InputIterator last)
{

    if (first == last)
        return std::make_pair(0, 0);

    Result min = *first;
    Result max = min;
    InputIterator previous = first;

    while (++first != last)
    {
        InputIterator next = first;
        Result result = *previous - *next;
        previous = next;
        if (max < result)
            max = result;
        if (min > result)
            min = result;
    }

    return std::make_pair(min, max);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() != 2 )
    {
        std::cout << "Please give initial value, must be greater than 1." << std::endl;
        std::cout << "Usage: ./testSequence 100" << std::endl;
        return 1;
    }

    int value = atoi(args[1].c_str());
    if (value < 2)
    {
        std::cout << "Initial value must be greater than 1." << std::endl;
        return 1;
    }

    std::vector<long long> series; // store long long cos 3*n + 1 might overflow for big 'n'.

    while (true)
    {
        series.push_back(value);
        if (value ==1) break;
        value = generator(value);
    }

    //1. Maximum number in the series
    std::vector<long long>::iterator series_iter = std::max_element(series.begin(), series.end());
    std::cout << *series_iter << std::endl;
    
    //2. Zero based index in the series of the maximum number
    std::cout << std::distance(series.begin(), series_iter) << std::endl;

    //3. Maximum difference between successive numbers in the series
    std::vector<long long> tmp(series.size(), 0);
    std::adjacent_difference(series.begin(), series.end(), tmp.begin(), difference<long long>());
    std::cout << *std::max_element(tmp.begin() + 1, tmp.end()) << std::endl;
    
    //4. Minimum difference between successive numbers in the series
    std::cout << *std::min_element(tmp.begin() + 1, tmp.end()) << std::endl;
    
    //5. Total of all of the numbers in the series
    std::cout << std::accumulate(series.begin(), series.end(), 0LL) << std::endl; // use 0LL(long long) to reduce the possibility of overflow
    
    //6. List the numbers in the series that are wholly divisible by 5 in the order that they appear
    std::for_each(series.begin(), series.end(), printDivisibleByFive());
    std::cout << std::endl;

    return 0;
}


/*
 *  Comments:
 *
 * For questions 3 and 4, we could be more memory effecient by doing in place calculation of the difference if 'series' container was/could be mutable. 
 * i.e. std::adjacent_difference(series.begin(), series.end(), series.begin(), difference<long long>());
 * If that's not good enough, we have to call it in a very performance critical part of the code and we want only min/max we can calculate 
 * with a custom loop like custom_adjacent_difference.
 *   
 *  std::pair<long long, long long> res = custom_adjacent_difference<std::vector<long long>::iterator, long long>(series.begin()+1, series.end());
 *  std::cout << res.second << ',' << res.first << std::endl;
 */

