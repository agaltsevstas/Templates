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
