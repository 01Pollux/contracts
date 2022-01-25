#pragma once

#include <stdexcept>

namespace px
{
    class precond_exception : public std::exception
    {
    public:
        precond_exception(const char* msg) noexcept : std::exception(msg) {}
    };

    class postcond_exception : public std::exception
    {
    public:
        postcond_exception(const char* msg) noexcept : std::exception(msg) {}
    };

    class assert_exception : public std::exception
    {
    public:
        assert_exception(const char* msg) noexcept : std::exception(msg) {}
    };

    class empty_contract {};
    template<typename _Ty>
    static constexpr bool is_empty_contract_v = std::is_same_v<empty_contract, _Ty>;

    template<typename _PreCondTy, typename _PostCondTy>
    class conditional_contract
    {
    public:
        using precondition_type = _PreCondTy;
        using postcondition_type = _PostCondTy;

        constexpr explicit conditional_contract(precondition_type&& pre_cond, postcondition_type&& post_cond)  noexcept(is_empty_contract_v<precondition_type>&& std::is_nothrow_move_constructible_v<postcondition_type>) :
            m_PostCond(std::move(post_cond))
        {
            if constexpr (!is_empty_contract_v<precondition_type>)
            {
                if (!pre_cond())
                    throw precond_exception(pre_cond.what());
            }
        }

        constexpr explicit conditional_contract(postcondition_type post_cond)  noexcept(std::is_nothrow_move_constructible_v<postcondition_type>) :
            conditional_contract(empty_contract{ }, std::move(post_cond))
        {}

        constexpr explicit conditional_contract(precondition_type pre_cond)  noexcept(is_empty_contract_v<precondition_type>) :
            conditional_contract(std::move(pre_cond), empty_contract{ })
        {}

        _CONSTEXPR20 ~conditional_contract() noexcept (is_empty_contract_v<postcondition_type>)
        {
            if constexpr (!is_empty_contract_v<postcondition_type>)
            {
                if (!m_PostCond())
                    throw precond_exception(m_PostCond.what());
            }
        }

        conditional_contract(const conditional_contract&) = delete;
        conditional_contract& operator=(const conditional_contract&) = delete;
        conditional_contract(conditional_contract&&) = delete;
        conditional_contract& operator=(conditional_contract&&) = delete;

    private:
        postcondition_type m_PostCond{ };
    };

    template<typename _CondTy>
    class assert_contract
    {
    public:
        constexpr explicit assert_contract(_CondTy assert_cond)
        {
            if constexpr (!is_empty_contract_v<_CondTy>)
            {
                if (!assert_cond())
                    throw assert_exception(assert_cond.what());
            }
        }
    };

    template<typename _CallbackTy>
    class contract_object
    {
    public:
        using call_or_result_type = _CallbackTy;
        static constexpr bool is_immediate_result = std::is_same_v<call_or_result_type, bool>;

        constexpr contract_object(call_or_result_type callback, const char* msg)
            noexcept(is_immediate_result || std::is_nothrow_move_constructible_v<call_or_result_type>) :
            m_Callback(std::move(callback)),
            m_Message(msg)
        {}

        constexpr bool operator()() const noexcept(is_immediate_result || std::is_nothrow_invocable_r_v<bool, call_or_result_type>)
        {
            if constexpr (std::is_same_v<call_or_result_type, bool>)
                return m_Callback;
            else return m_Callback();
        }

        constexpr const char* what() const noexcept
        {
            return m_Message;
        }

    private:
        const char* m_Message;
        _CallbackTy m_Callback;
    };


    template<typename _Ty>
    using precond_contract = conditional_contract<contract_object<_Ty>, empty_contract>;

    template<typename _Ty>
    using postcond_contract = conditional_contract<empty_contract, contract_object<_Ty>>;

    template<typename _CondTy>
    constexpr void expects(_CondTy&& pred, const char* msg = "pre-condition exception")
    {
        precond_contract expect_tmp(contract_object{ std::move(pred), msg });
    }

    template<typename _CondTy>
    [[nodiscard]] constexpr const postcond_contract<_CondTy> ensures(_CondTy&& pred, const char* msg = "post-condition exception")
    {
        return postcond_contract(contract_object{ std::move(pred), msg });
    }

#define PX_MAKE_STRING_(X) #X
#define PX_MAKE_STRING(X) PX_MAKE_STRING_(X)

#define PX_LINE_STR     PX_MAKE_STRING(__LINE__)

#define PX_THISFILE_LINE(Message)   \
    "Exception reported:\n"         \
    "File: " __FILE__ "\n"      \
    "Line: " PX_LINE_STR "\n"       \
    "Message: " ##Message


#ifdef _DEBUG
    template<typename _CondTy>
    constexpr auto assert_(_CondTy&& pred, const char* msg = "assert exception")
    {
        return assert_contract(contract_object{ std::move(pred), msg });
    }
#else
    constexpr auto assert_(...)
    {
        return assert_contract(empty_contract{});
    }
#endif
}
