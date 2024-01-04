#include "Instantiation.h"

#include <string>

namespace instantiation
{
    template<typename ...TArgs>
    constexpr int Instantiation::explicitCountArgs(TArgs ...args)
    {
        return sizeof...(args);
    }

    template int Instantiation::explicitCountArgs(const int);
    template int Instantiation::explicitCountArgs(const double);
    template int Instantiation::explicitCountArgs(const std::string);
    template int Instantiation::explicitCountArgs(const int, const double, const std::string);
}
