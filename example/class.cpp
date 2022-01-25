#include "../include/px/contracts.hpp"

#include <iostream>
#include <vector>
#include <algorithm>


/// <summary>
/// 
/// px::postcond_contract<no_null_pointer>:
/// 
/// a 'postcond_contract' contract wrapped with a user defined 'no_null_pointer' struct
/// 'no_null_pointer' ensures that the parent's pointer 'somepointer'
/// 
/// when 'SomeClass' desctructor gets called, each member variable should be destroyed in reverse order they were created with
/// therefore a member variable post-contract should be declared before the target variable in this case 'somepointer'
/// 
/// </summary>
class SomeClass
{
public:
    SomeClass(size_t entries)
    {
        px::expects(entries > 0, PX_THISFILE_LINE("SomeClass(entries < 0)"));

        m_V.reserve(entries);
        for (size_t i = 0; i < entries; i++)
            m_V.emplace_back(i);

        somepointer = new size_t(accumulate());
    }

    ~SomeClass()
    {
        delete somepointer;
        // Uncommenting the line below will throw an exception from 'px::postcond_contract<no_null_pointer>'
        somepointer = nullptr;
    }

    size_t size() const noexcept
    {
        return m_V.size();
    }

    size_t accumulate() const noexcept
    {
        size_t sum = 0;
        for (const size_t x : m_V)
            sum += x;
        return sum;
    }

private:
    std::vector<size_t> m_V;

    struct no_null_pointer
    {
        constexpr no_null_pointer(SomeClass* parent) noexcept : _Parent(parent) {}

        bool operator()() const noexcept
        {
            return _Parent->somepointer == nullptr;
        }

        const char* what() const noexcept
        {
            return "_Parent->somepointer wasn't properly deleted/set to nullptr";
        }

        SomeClass* _Parent;
    };
    px::postcond_contract<no_null_pointer> _NoNull = px::ensures(no_null_pointer{ this });

    size_t* somepointer{ };
};

int main()
{
    {
        SomeClass c(10);
        px::assert_(
            [&c]()
            {
                if (c.size() != 10)
                    return false;
                return c.accumulate() == 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9;
            }
        );
    }
}
