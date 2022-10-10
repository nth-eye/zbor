#include <iostream>
#include "utl/time.h"
#include "utl/vector.h"
#include "utl/ring.h"
#include "utl/pool.h"
#include "utl/log.h"
#include "utl/makima.h"

struct X {
    X() { 
        puts("default constructor"); 
    }
    X(int y) {
        puts("argument constructor");
        x = y;
    }
    ~X() {
        puts("destructor");
        x = -99;
    }
    X(const X& rhs) noexcept { 
        puts("copy constructor");
        x = rhs.x;
    }
    X(X&& rhs) noexcept { 
        puts("move constructor");
        x = rhs.x;
    }
    X& operator=(const X& rhs) noexcept {
        puts("copy assignment");
        x = rhs.x;
        return *this;
    }
    X& operator=(X&& rhs) noexcept {
        puts("move assignment");
        x = rhs.x;
        return *this;
    }
public:
    int x = 0;
};

int main()
{
    // X x = 2;
    // using pool_t = utl::pool<X, 4>;
    // using vect_t = utl::vector<pool_t::return_type, 5>;

    // printf("sizeof(utl::pool<X, 1>):                %lu \n", sizeof(utl::pool<X, 1>));
    // printf("sizeof(utl::pool<X, 1>::return_type):   %lu \n", sizeof(utl::pool<X, 1>::return_type));

    // pool_t obj_pool;
    // {
    //     vect_t obj_vect;

    //     std::cout << "+----starting----+" << std::endl;

    //     obj_vect.push_back(obj_pool.get());
    //     obj_vect.push_back(obj_pool.get(1));
    //     obj_vect.push_back(obj_pool.get(x));
    //     obj_vect.push_back(obj_pool.get(std::move(x)));
    //     obj_vect.push_back(obj_pool.get(4));

    //     std::cout << "+---------------+" << std::endl;
    //     [&] () {
    //         for (auto& obj : obj_vect)
    //             printf("[%p] %d \n", (void*) obj.get(), obj ? obj->x : 0);
    //     }();
    //     std::cout << "+---------------+" << std::endl;
    // }
    // std::cout << "+------exiting------+" << std::endl;

    std::cout << "+-------------------+" << std::endl;

    using word_type = uint32_t;
    static constexpr size_t grow_point = 4;

    utl::print_bit_tree_info<word_type, 64, grow_point>();
    utl::print_bit_tree_info<word_type, 32, grow_point>();
    utl::print_bit_tree_info<word_type, 128, grow_point>();
    utl::print_bit_tree_info<word_type, 129, grow_point>();
    utl::print_bit_tree_info<word_type, 991, grow_point>();
    utl::print_bit_tree_info<word_type, 992, grow_point>();
    utl::print_bit_tree_info<word_type, 993, grow_point>();
    utl::print_bit_tree_info<word_type, 1024, grow_point>();
    utl::print_bit_tree_info<word_type, 32768, grow_point>();
    utl::print_bit_tree_info<word_type, 1048576, grow_point>();
    utl::print_bit_tree_info<word_type, 33554432, grow_point>();
    utl::print_bit_tree_info<word_type, 1073741824, grow_point>();
    utl::print_bit_tree_info<word_type, 4294967296, grow_point>();

    std::cout << "+-------------------+" << std::endl;

    // constexpr auto ss = 1280;
    // utl::pool<int, ss, 4, uint32_t> pool;
    // utl::vector<decltype(pool)::return_type, ss> vector;

    // auto test_1 = [&]() {
    //     for (int i = 0; i < ss; ++i)
    //         vector.push_back(pool.get(i));
    //     vector.clear();
    // };
    // auto test_2 = [&]() {
    //     for (int i = 0; i < ss; ++i) {
    //         vector_flat.push_back(pool_flat.get(i));
    //     }
    //     vector_flat.clear();
    // };
    // constexpr size_t count = 1000000; 

    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_1));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_1));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_1));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_1));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_2));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_2));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_2));
    // printf(": %3ld clock_t\n", utl::exec_time<count>(test_2));


    utl::makima<double, 16> fsr_map = {
        {   90,     112,    155,    240,    340,    550,    800,    1000,
            1300,   2500,   5000,   55000,  100000, 115000, 120000, 1000000 },
        {   45.0,   25.0,   20.0,   17.5,   15.0,   12.5,   10.0,   8.75,
            7.5,    6.25,   5.0,    3.75,   2.5,    1.25,   0.03,   0.02    },
    };
    auto val = double(299);
    auto ret = fsr_map(val);
    if (!ret) {
        printf("fsr_map(%.2f):    invalid \n", val);
    } else {
        printf("fsr_map(%.2f):    %.2f \n", val, *ret);
    }
}
