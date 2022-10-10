#ifndef _CONTAINER_TESTER_H
#define _CONTAINER_TESTER_H

#include <gtest/gtest.h>
#include <cassert>

struct tester {
    tester() : self(this), x(0) 
    {
        ++livecnt;
    }
    tester(int x) : self(this), x(x) 
    {
        ++livecnt;
    }
    tester(const tester& other) : self(this), x(other.x)
    {
        ++livecnt;
        ++copycnt;
    }
    tester(tester&& other) noexcept : self(this), x(std::move(other.x))
    {
        ++livecnt;
        ++movecnt;
    }
    ~tester() 
    {
        --livecnt;
        assert(self == this);
    }
    tester& operator=(const tester& other) 
    {
        ++copycnt;
        x = other.x;
        assert(self == this);
        assert(other.self == &other);
        return *this;
    }
    tester& operator=(tester&& other) noexcept
    {
        ++movecnt;
        x = std::move(other.x);
        assert(self == this);
        assert(other.self == &other);
        return *this;
    }
    int operator()() const
    {
        return x;
    }
    static size_t livecount()
    {
        return livecnt;
    }
    static size_t movecount()
    {
        return movecnt;
    }
    static size_t copycount()
    {
        return copycnt;
    }
    static void clear()
    {
        livecnt = 0;
        copycnt = 0;
        movecnt = 0;
    }
private:
    inline static size_t livecnt = 0;
    inline static size_t copycnt = 0;
    inline static size_t movecnt = 0;
    const tester* self;
    int x;
};

class ContainerTester : public ::testing::Test {
protected:
    void SetUp() override 
    {
        tester::clear();
    }
    void TearDown() override 
    {
        ASSERT_EQ(tester::livecount(), 0);
    }
};

inline void verify_count(size_t copy, size_t move)
{
    ASSERT_EQ(tester::copycount(), copy);
    ASSERT_EQ(tester::movecount(), move);
}

#endif