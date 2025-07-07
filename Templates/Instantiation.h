#ifndef INSTANTIATION_hpp
#define INSTANTIATION_hpp

#include <iostream>

namespace instantiation
{
    class Instantiation
    {
    public:
        template<typename ...TArgs>
        inline constexpr int ImplicitCountArgs(TArgs&& ...args)
        {
            return sizeof...(args);
        }
        
        template<typename ...TArgs>
        constexpr int ExplicitCountArgs(TArgs ...args);
    };

    extern template int Instantiation::ExplicitCountArgs(int);
    extern template int Instantiation::ExplicitCountArgs(int&);
    extern template int Instantiation::ExplicitCountArgs(int&&);
    extern template int Instantiation::ExplicitCountArgs(double&);
    extern template int Instantiation::ExplicitCountArgs(double&&);
    extern template int Instantiation::ExplicitCountArgs(std::string&);
    extern template int Instantiation::ExplicitCountArgs(std::string&&);
    extern template int Instantiation::ExplicitCountArgs(int&, double&, std::string&);
    extern template int Instantiation::ExplicitCountArgs(int&&, double&&, std::string&&);
}
#endif /* INSTANTIATION_hpp */
