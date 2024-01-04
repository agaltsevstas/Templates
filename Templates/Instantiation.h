#ifndef INSTANTIATION_hpp
#define INSTANTIATION_hpp

#include <iostream>

namespace instantiation
{
    class Instantiation
    {
    public:
        template<typename ...TArgs>
        inline constexpr int implicitCountArgs(TArgs ...args)
        {
            return sizeof...(args);
        }
        
        template<typename ...TArgs>
        constexpr int explicitCountArgs(TArgs ...args);
    };
}
#endif /* INSTANTIATION_hpp */
