#include "../include/px/contracts.hpp"

/// https://github.com/boostorg/contract/blob/master/example/features/check.cpp
constexpr int gcd(int a, int b)
{
    px::expects(a < b , PX_THISFILE_LINE("a >= b"));

    int x = a, y = b;
    while (x != y)
    {
        if (x > y) x = x - y;
        else y = y - x;
    }
    
    auto post_cond = px::ensures(
        [x, a, b]()
        {
            return x < a && x < b;
        },
        PX_THISFILE_LINE("!(a < x < b)")
    );
    return x;
}

int main()
{
    static_assert(gcd(12, 28) == 4);
    static_assert(gcd(10, 15) == 5);
    static_assert(gcd(4, 14) == 2);

    px::assert_(gcd(12, 28) == 4, PX_THISFILE_LINE("calc(12, 28) != 4"));
    px::assert_(gcd(10, 15) == 5, PX_THISFILE_LINE("gcd(10, 15) != 5"));
    px::assert_(gcd(4, 14) == 2, PX_THISFILE_LINE("calc(4, 14) != 2"));
}
