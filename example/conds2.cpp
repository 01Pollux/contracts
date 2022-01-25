#include "../include/px/contracts.hpp"
#include <iostream>

void make_negative(int& v)
{
    px::expects(v >= 0, PX_THISFILE_LINE("v < 0"));
    auto post_cond = px::ensures(
        [&v]() -> bool { return v <= 0; },
        PX_THISFILE_LINE("v > 0")
    );

    v = -v;
}

int main()
{
    int x = 0;
    std::cout << x << " -> ";
    make_negative(x);
    px::assert_(x <= 0);
    std::cout << x << '\n';

    x = 10;
    std::cout << x << " -> ";
    make_negative(x);
    px::assert_(x <= 0);
    std::cout << x << '\n';

    x = -10;
    std::cout << x << " -> ";
    try
    {
        px::assert_(x > 0);
        make_negative(x);
        px::assert_(x <= 0);
        std::cout << x << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << x << '\n';
        std::cout << ex.what() << '\n';
    }
}
