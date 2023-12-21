#ifndef Function_h
#define Function_h

namespace function
{
    class Base
    {

    };

    class Derived1 : public Base
    {
    public:
        void Method() {}
        void Derived1Method() {};
    };

    class Derived2 : public Base
    {
    public:
        void Method() {}
        void Derived2Method() {};
    };

    template < class T >
    class HasMember_FocusSetEvent
    {
    private:
        using Yes = char[2];
        using  No = char[1];

        struct Fallback { int FocusSetEvent; };
    //    struct Derived : T, Fallback { };

        template < class U >
        static No& test(decltype(U::FocusSetEvent)*);
        template < typename U >
        static Yes& test(U*);

    public:
        static constexpr bool RESULT1 = sizeof(test<Derived1>(nullptr)) == sizeof(Yes);
        static constexpr bool RESULT2 = sizeof(test<Derived2>(nullptr)) == sizeof(Yes);
    };

    template < class T >
    struct has_member_FocusSetEvent
        : public std::integral_constant<bool, HasMember_FocusSetEvent<T>::RESULT>
    {
    };

    template <class Arg, class ... Args>
    constexpr int countArgs(const Arg& x, const Args& ... args)
    {
        return countArgs(args...) + 1;
    }

    template <typename R, typename T, typename ... Types>
    constexpr std::integral_constant<unsigned, sizeof ...(Types)> GetArgsCount(R(T::*)(Types ...))
    {
        return static_cast<int>(std::integral_constant<unsigned, sizeof ...(Types)>{});
    }

    template <typename DerivedT>
    void CallFunction1(std::shared_ptr<Base>& iDerived)
    {
        static_cast<DerivedT&>(*iDerived).Method();
    }

    template <typename PointerT, typename DerivedT>
    void CallFunction1(DerivedT& iDerived)
    {
        iDerived->Method();
    }

    template <typename DerivedT>
    std::shared_ptr<DerivedT> CallFunction2(std::shared_ptr<DerivedT>& iDerived)
    {
        iDerived->Method();
        return iDerived;
    }

    // SFINAE вместе с constexpr
    template <typename DerivedT>
    void CallFunction3(std::shared_ptr<DerivedT>& iDerived)
    {
        // Общий метод для 2 классов
        iDerived->Method();
        if constexpr (std::is_base_of<Derived1, DerivedT>::value)
        {
            iDerived->Derived1Method();
        }
        else if constexpr (std::is_base_of<Derived2, DerivedT>::value)
        {
            iDerived->Derived2Method();
        }
    }
}

#endif /* Function_h */
