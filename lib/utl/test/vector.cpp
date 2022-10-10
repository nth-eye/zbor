#include <gtest/gtest.h>
#include "utl/vector.h"
#include "_container_tester.h"

static constexpr size_t test_size = 5;

using Vec = utl::vector<tester, test_size>;

static void verify_vec(const Vec& v, size_t copy, size_t move, std::initializer_list<int> arr)
{
    size_t size = arr.size();

    ASSERT_LE(size, test_size);
    ASSERT_EQ(v.capacity(), test_size);
    ASSERT_EQ(v.max_size(), test_size);
    ASSERT_EQ(v.size(), size);
    ASSERT_EQ(v.full(), size == test_size);
    ASSERT_EQ(v.empty(), size == 0);
    ASSERT_EQ(v.begin() + size, v.end());
    ASSERT_EQ(v.begin(), v.data());

    if (v.empty() == false) {
        ASSERT_EQ(&v.front(), v.begin());
        ASSERT_EQ(&v.back(), v.end() - 1);
    }
    verify_count(copy, move);

    size_t i = 0;
    for (auto it : arr)
        ASSERT_EQ(v[i++](), it) << "at index " << i;
}

struct Vector : ContainerTester {
    void TearDown() override 
    {
        v.clear();
        ContainerTester::TearDown();
    }
    void setup(std::initializer_list<int> arr)
    {
        ASSERT_LE(arr.size(), test_size);
        v.clear();
        for (auto it : arr)
            v.emplace_back(it);
    }
    void verify(size_t copy, size_t move, std::initializer_list<int> arr)
    {
        verify_vec(v, copy, move, arr);
    }
    Vec v;
};

TEST_F(Vector, ConstructorDefault)
{
    Vec x;
    verify_vec(x, 0, 0, {});
}

TEST_F(Vector, ConstructorDefaultN)
{
    Vec x(4);
    verify_vec(x, 0, 0, {0, 0, 0, 0});
}

TEST_F(Vector, ConstructorConstReferenceN)
{
    Vec x(4, 77);
    verify_vec(x, 4, 0, {77, 77, 77, 77});
}

TEST_F(Vector, ConstructorRange)
{
    setup({22, 33, 44});
    Vec x(v.begin(), v.begin() + 2);
    verify_vec(x, 2, 0, {22, 33});
    verify_vec(v, 2, 0, {22, 33, 44}); // verify source didn't change
}

TEST_F(Vector, CopyConstructor)
{
    setup({66, 66, 66});
    Vec x = v;
    verify_vec(x, 3, 0, {66, 66, 66});
    verify_vec(v, 3, 0, {66, 66, 66});
}

TEST_F(Vector, MoveConstructor)
{
    Vec y(2, 55);
    Vec x = std::move(y);
    verify_vec(x, 2, 2, {55, 55});
}

// TEST_F(Vector, ConstructorIlist)
// {
// TODO
// }

TEST_F(Vector, ConstructorStdArrayConstReference)
{
    const std::array<tester, 0> arr_0 = {};
    Vec x = arr_0;
    verify_vec(x, 0, 0, {});

    const std::array<tester, 2> arr_1 = {100, 200};
    Vec y = arr_1;
    verify_vec(y, 2, 0, {100, 200});

    const std::array<tester, 5> arr_2 = {999};
    Vec z = arr_2;
    verify_vec(z, 7, 0, {999, 0, 0, 0, 0});
}

TEST_F(Vector, ConstructorArrayUniversalReference)
{
    Vec x = {{1, 1}};
    verify_vec(x, 0, 2, {1, 1});

    Vec y = {{3, 2, 1, 0, -1}};
    verify_vec(y, 0, 7, {3, 2, 1, 0, -1});
}

TEST_F(Vector, AssignmentOperatorCopy)
{
    const Vec src(4, 4);
    v = src;
    verify(8, 0, {4, 4, 4, 4});
}

TEST_F(Vector, AssignmentOperatorMove)
{
    v = Vec(3, 3); 
    verify(3, 3, {3, 3, 3});
}

// TEST_F(Vector, AssignmentOperatorIlist)
// {
// TODO
// }

TEST_F(Vector, AssignmentOperatorStdArrayConstReference)
{
    const std::array<tester, 2> arr_0 = {1, 2};
    v = arr_0;
    verify(2, 0, {1, 2});

    const std::array<tester, 5> arr_1 = {5, 4, 3, 2, 1};
    v = arr_1;
    verify(7, 0, {5, 4, 3, 2, 1});

    const std::array<tester, 0> arr_2 = {};
    v = arr_2;
    verify(7, 0, {});
}

TEST_F(Vector, AssignmentOperatorArrayUniversalReference)
{
    v = {666, 1, 2, 3, 4};
    verify(0, 5, {666, 1, 2, 3, 4});

    v = {tester(1), 2};
    verify(0, 7, {1, 2});

    v = {tester(42)};
    verify(0, 8, {42});
}

TEST_F(Vector, AssignN)
{
    v.assign(2, 2);
    verify(2, 0, {2, 2});

    v.assign(5, 5);
    verify(7, 0, {5, 5, 5, 5, 5});

    v.assign(0, 0);
    verify(7, 0, {});
}

TEST_F(Vector, AssignRange)
{
    std::array<tester, test_size> arr = {1, 2, 3, 4, 5};

    v.assign(arr.begin(), arr.begin() + 2);
    verify(2, 0, {1, 2});

    v.assign(arr.begin(), arr.end());
    verify(7, 0, {1, 2, 3, 4, 5});

    v.assign(arr.begin(), arr.begin());
    verify(7, 0, {});
}

// TEST_F(Vector, AssignIlist)
// {
// TODO
// }

TEST_F(Vector, AssignStdArrayConstReference)
{
    const std::array<tester, 2> arr_0 = {1, 2};
    v.assign(arr_0);
    verify(2, 0, {1, 2});

    const std::array<tester, 5> arr_1 = {5, 4, 3, 2, 1};
    v.assign(arr_1);
    verify(7, 0, {5, 4, 3, 2, 1});

    const std::array<tester, 0> arr_2 = {};
    v.assign(arr_2);
    verify(7, 0, {});
}

TEST_F(Vector, AssignArrayUniversalReference)
{
    tester arr[1] = {666};

    v.assign(std::move(arr));
    verify(0, 1, {666});

    v.assign({1, 2});
    verify(0, 3, {1, 2});

    v.assign({5, 4, 3, 2, 1});
    verify(0, 8, {5, 4, 3, 2, 1});
}

TEST_F(Vector, ResizeDefault)
{
    v.resize(2);
    verify(0, 0, {0, 0});

    v.resize(5);
    verify(0, 0, {0, 0, 0, 0, 0});

    v.resize(0);
    verify(0, 0, {});
}

TEST_F(Vector, ResizeReference)
{
    v.resize(2, 0);
    verify(2, 0, {0, 0});

    v.resize(3, 3);
    verify(3, 0, {0, 0, 3});

    v.resize(5, 5);
    verify(5, 0, {0, 0, 3, 5, 5});

    v.resize(0, {});
    verify(5, 0, {});
}

TEST_F(Vector, Clear)
{
    v.resize(5);
    v.clear();
    verify(0, 0, {});
}

TEST_F(Vector, PushBack)
{
    const tester obj = 1;

    v.push_back(obj);
    v.push_back(2);
    v.push_back({});

    verify(1, 2, {1, 2, 0});
}

TEST_F(Vector, EmplaceBack)
{
    tester obj = 10;

    v.emplace_back(obj);
    v.emplace_back(std::move(obj));
    v.emplace_back(13);
    v.emplace_back();

    verify(1, 1, {10, 10, 13, 0});
}

TEST_F(Vector, Emplace)
{
    setup({1, 2});

    v.emplace(v.begin() + 1, 3);
    verify(0, 1, {1, 3, 2});
    v.emplace(v.end(), 4);
    verify(0, 1, {1, 3, 2, 4});
    v.emplace(v.begin(), 5);
    verify(0, 5, {5, 1, 3, 2, 4});
}

TEST_F(Vector, InsertConstReference)
{
    const tester obj = 10;

    setup({1, 2});

    v.insert(v.begin() + 1, obj);
    verify(1, 1, {1, 10, 2});
    v.insert(v.end(), obj);
    verify(2, 1, {1, 10, 2, 10});
    v.insert(v.begin(), obj);
    verify(3, 5, {10, 1, 10, 2, 10});
}

TEST_F(Vector, InsertUniversalReference)
{
    setup({1, 2});

    v.insert(v.begin() + 1, 10);
    verify(0, 2, {1, 10, 2});
    v.insert(v.end(), 10);
    verify(0, 3, {1, 10, 2, 10});
    v.insert(v.begin(), 10);
    verify(0, 8, {10, 1, 10, 2, 10});
}

TEST_F(Vector, InsertN)
{
    const tester obj = 10;

    setup({1, 2, 3});
    v.insert(v.begin() + 1, 2, obj);
    verify(2, 2, {1, 10, 10, 2, 3});

    setup({1, 2, 3});
    v.insert(v.begin() + 2, 2, obj);
    verify(4, 3, {1, 2, 10, 10, 3});

    setup({1, 2, 3});
    v.insert(v.end(), 2, obj);
    verify(6, 3, {1, 2, 3, 10, 10});

    setup({1, 2, 3});
    v.insert(v.begin(), 2, obj);
    verify(8, 6, {10, 10, 1, 2, 3});
}

TEST_F(Vector, InsertRange)
{
    std::array<tester, 2> arr = {10, 11}; 

    setup({1, 2, 3});
    v.insert(v.begin() + 1, arr.begin(), arr.end());
    verify(2, 2, {1, 10, 11, 2, 3});

    setup({1, 2, 3});
    v.insert(v.begin() + 2, arr.begin(), arr.end());
    verify(4, 3, {1, 2, 10, 11, 3});

    setup({1, 2, 3});
    v.insert(v.end(), arr.begin(), arr.end());
    verify(6, 3, {1, 2, 3, 10, 11});

    setup({1, 2, 3});
    v.insert(v.begin(), arr.begin(), arr.end());
    verify(8, 6, {10, 11, 1, 2, 3});

    setup({1, 2, 3});
    v.insert(v.begin(), arr.begin(), arr.begin());
    verify(8, 6, {1, 2, 3});
}

TEST_F(Vector, InsertIlist)
{
    setup({1, 2, 3});
    v.insert(v.begin() + 1, {10, 11});
    verify(2, 2, {1, 10, 11, 2, 3});

    setup({1, 2, 3});
    v.insert(v.begin() + 2, {10, 11});
    verify(4, 3, {1, 2, 10, 11, 3});

    setup({1, 2, 3});
    v.insert(v.end(), {10, 11});
    verify(6, 3, {1, 2, 3, 10, 11});

    setup({1, 2, 3});
    v.insert(v.begin(), {10, 11});
    verify(8, 6, {10, 11, 1, 2, 3});
}

TEST_F(Vector, PopBack)
{
    setup({1, 2, 3});
    v.pop_back();
    verify(0, 0, {1, 2});
    v.pop_back();
    verify(0, 0, {1});
}

TEST_F(Vector, PopIdx)
{
    setup({1, 2, 3, 4, 5});
    v.pop_idx(0);
    verify(0, 1, {5, 2, 3, 4});
    v.pop_idx(1);
    verify(0, 2, {5, 4, 3});
    v.pop_idx(2);
    verify(0, 2, {5, 4});
}

TEST_F(Vector, PopSingle)
{
    setup({1, 2, 3, 4, 5});
    v.pop(v.begin());
    verify(0, 1, {5, 2, 3, 4});
    v.pop(v.begin() + 1);
    verify(0, 2, {5, 4, 3});
    v.pop(v.begin() + 2);
    verify(0, 2, {5, 4});
}

TEST_F(Vector, PopRange)
{
    setup({1, 2, 3, 4, 5});
    v.pop(v.begin(), v.begin() + 2);
    verify(0, 2, {4, 5, 3});

    setup({1, 2, 3, 4, 5});
    v.pop(v.begin() + 3, v.begin() + 5);
    verify(0, 2, {1, 2, 3});

    setup({1, 2, 3, 4, 5});
    v.pop(v.begin() + 1, v.begin() + 3);
    verify(0, 4, {1, 4, 5});

    setup({1, 2, 3, 4, 5});
    v.pop(v.begin() + 2, v.begin() + 4);
    verify(0, 5, {1, 2, 5});
}

TEST_F(Vector, EraseSingle)
{
    setup({1, 2, 3, 4, 5});
    v.erase(v.begin());
    verify(0, 4, {2, 3, 4, 5});

    setup({1, 2, 3, 4, 5});
    v.erase(v.end() - 1);
    verify(0, 4, {1, 2, 3, 4});

    setup({1, 2, 3, 4, 5});
    v.erase(v.end() - 3);
    verify(0, 6, {1, 2, 4, 5});
}

TEST_F(Vector, EraseRange)
{
    setup({1, 2, 3, 4, 5});
    v.erase(v.begin(), v.begin() + 2);
    verify(0, 3, {3, 4, 5});

    setup({1, 2, 3, 4, 5});
    v.erase(v.end() - 2, v.end());
    verify(0, 3, {1, 2, 3});

    setup({1, 2, 3, 4, 5});
    v.erase(v.begin() + 1, v.begin() + 3);
    verify(0, 5, {1, 4, 5});

    setup({1, 2, 3, 4, 5});
    v.erase(v.begin() + 2, v.begin() + 4);
    verify(0, 6, {1, 2, 5});
}

TEST_F(Vector, Swap)
{

}
