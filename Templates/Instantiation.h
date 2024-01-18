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
}
#endif /* INSTANTIATION_hpp */
