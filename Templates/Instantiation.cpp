#include "Instantiation.h"

#include <string>

namespace instantiation
{
    template<typename ...TArgs>
    constexpr int Instantiation::ExplicitCountArgs(TArgs ...args)
    {
        return sizeof...(args);
    }

    template int Instantiation::ExplicitCountArgs(int);
    template int Instantiation::ExplicitCountArgs(int&);
    template int Instantiation::ExplicitCountArgs(int&&);
    template int Instantiation::ExplicitCountArgs(double&);
    template int Instantiation::ExplicitCountArgs(double&&);
    template int Instantiation::ExplicitCountArgs(std::string&);
    template int Instantiation::ExplicitCountArgs(std::string&&);
    template int Instantiation::ExplicitCountArgs(int&, double&, std::string&);
    template int Instantiation::ExplicitCountArgs(int&&, double&&, std::string&&);
}
