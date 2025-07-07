#ifndef invoke_apply_h
#define invoke_apply_h

namespace invoke_apply
{
    void print(const auto&... args)
    {
        size_t index = sizeof...(args);
        auto print = [&index](auto&& x)
        {
            std::cout << x;
            if (index-- > 1)
                std::cout << ", ";
            else
                std::cout << std::endl;
        };
        
        (print(std::forward<decltype(args)>(args)), ...);
    }

    struct Print
    {
        int GetValue() noexcept { return value; }
        void SetValue(int iValue) noexcept { value = iValue; }
        
        void operator()(auto&&... args)
        {
            size_t index = sizeof...(args);
            auto print = [&index](auto&& x)
            {
                std::cout << x;
                if (index-- > 1)
                    std::cout << ", ";
                else
                    std::cout << std::endl;
            };

            (print(std::forward<decltype(args)>(args)), ...);
        }
        
        void print(const auto&... args)
        {
            size_t index = sizeof...(args);
            auto print = [&index](auto&& x)
            {
                std::cout << x;
                if (index-- > 1)
                    std::cout << ", ";
                else
                    std::cout << std::endl;
            };
            
            (print(std::forward<decltype(args)>(args)), ...);
        }
        
        int value;
    };

    template<typename TFunction, typename... TArgs>
    decltype(auto) CallInvoke(TFunction&& function, TArgs&& ...args)
    {
        // function(std::forward<TArgs>(args)...);
        return std::invoke(std::forward<TFunction>(function), std::forward<TArgs>(args)...);
    }

    template<typename TFunction, typename... TArgs>
    decltype(auto) CallApply(TFunction&& function, const std::tuple<TArgs...>& tuple)
    {
        return std::apply(std::forward<TFunction>(function), tuple);
    }
}


#endif /* invoke_apply_h */
