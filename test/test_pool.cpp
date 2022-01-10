#include <gtest/gtest.h>
#include "zbor_base.h"

using namespace zbor;

// class PoolOne : public ::testing::Test {
// protected:
//     void SetUp() override 
//     {
//     }
//     Pool<1> pool;
// };

// TEST_F(PoolOne, DefaultSize)
// {
//     EXPECT_EQ(pool.size(), 0);
// }

// TEST_F(PoolOne, SizeAfterClear)
// {
//     pool.clear();

//     EXPECT_EQ(pool.size(), 0);
// }

// TEST_F(PoolOne, MakeOne)
// {
//     EXPECT_NE(pool.make(), nullptr);
//     EXPECT_EQ(pool.size(), 1);
// }

// TEST_F(PoolOne, MakeMoreThanCapacity)
// {
//     EXPECT_NE(pool.make(), nullptr);
//     EXPECT_EQ(pool.make(), nullptr);
//     EXPECT_EQ(pool.size(), 1);
// }

// TEST_F(PoolOne, MakeAndFree)
// {
//     pool.free(pool.make());
//     EXPECT_EQ(pool.size(), 0);
// }

// TEST_F(PoolOne, DoubleFree)
// {
//     auto obj = pool.make();
//     pool.free(obj);
//     pool.free(obj);
//     EXPECT_EQ(pool.size(), 0);
// }

// TEST_F(PoolOne, FreeNullptr)
// {
//     pool.free(nullptr);
//     EXPECT_EQ(pool.size(), 0);
// }

// TEST_F(PoolOne, MakeAndFreeNullptr)
// {
//     pool.make();
//     pool.free(nullptr);
//     EXPECT_EQ(pool.size(), 1);
// }

// TEST_F(PoolOne, FreePointerOutOfBonds)
// {
//     CBOR wrong_obj;

//     pool.make();
//     pool.free(&wrong_obj);
//     EXPECT_EQ(pool.size(), 1);
// }

// TEST_F(PoolOne, FreeUnalignedPointer)
// {
//     ASSERT_GT(sizeof(CBOR), 1);

//     auto obj = pool.make();
//     auto raw = reinterpret_cast<char*>(obj) + 1;
//     auto ptr = reinterpret_cast<CBOR*>(raw);

//     pool.free(ptr);

//     EXPECT_EQ(pool.size(), 1);
// }