#ifndef CRTP_h
#define CRTP_h

/*
 Сайты: https://infotraining.bitbucket.io/cpp-adv/variadic-templates.html
 */


/*
 CRTP (curiously recurring template pattern) - рекурсивный шаблон: класс Derived наследуется от шаблонного класса Base, использующего Derived как шаблонный параметр T. Через метод базового класса Base можно вызвать метод наследуемого класса Derived, используя статический полиморфизм вместо динамического полиморфизма (без таблицы виртуальных функций): static_cast<T*>(this)->method()
 Плюсы:
 - делается во время компиляции (compile), а не во время исполнения (runtime)
 - нет накладных расходов на создание таблиц виртуальных функций
 - время, затрачиваемое на выбор метода в таблице виртуальных функций
 Минусы:
 - у всех классов-наследников должны определены методы, которые вызываются в базовом классе
*/

namespace CRTP
{
    template <typename T>
    class Base
    {
    public:
        void Interface1() // 1 Cпособ
        {
            static_cast<T*>(this)->Implementation1();
        };
        
        static void Interface2()
        {
            T::Implementation2(); // 2 Способ: static
        };
    };

    class Derived : public Base<Derived>
    {
    public:
        void Implementation1()
        {
            std::cout << "interface1 Derived" << std::endl;
        }
        
        static void Implementation2()
        {
            std::cout << "interface2 Derived" << std::endl;
        }
    };

    namespace SINGLETON
    {
        /*
         Миксины (Mixin): NonCopyable, NonMoveable
         */
        struct NonCopyable
        {
            NonCopyable() = default;
            NonCopyable(const NonCopyable&) = delete;
            NonCopyable& operator = (const NonCopyable&) = delete;
        };

        struct NonMoveable
        {
            NonMoveable() = default;
            NonMoveable(NonMoveable&&) = delete;
            NonMoveable& operator = (NonCopyable&&) = delete;
        };

        /*
         Синглтон Майерса + CRTP
         */
        template<class Derived>
        struct Singleton : private NonCopyable, private NonMoveable // Делаем приватные конструкторы базовых классов
        {
            static Derived &Instance() noexcept
            {
                static Derived instance;
                return instance;
            }
            
        protected:
            Singleton() = default;
        };

        struct Singleton1 : public Singleton<Singleton1>{};
        struct Singleton2 : public Singleton<Singleton2>{};
    }

    template <typename T>
    class Counter
    {
    public:
        Counter() noexcept { ++_counter; }
        ~Counter() noexcept { --_counter; }
        static size_t count() noexcept { return _counter; }
    private:
        inline static size_t _counter;
    };

    template <typename T>
    class Equal
    {
        friend bool operator==(const T& lhs, const T& rhs) { return lhs.equal_to(rhs); }
        friend bool operator!=(const T& lhs, const T& rhs) { return !lhs.equal_to(rhs); }
    };

    template <typename T>
    class Compare
    {
        friend bool operator<(const T& lhs, const T& rhs) { return lhs.less_than(rhs); }
        friend bool operator<=(const T& lhs, const T& rhs) { return !lhs.less_than(rhs); }
        friend bool operator>(const T& lhs, const T& rhs) { return lhs.less_than(rhs); }
        friend bool operator>=(const T& lhs, const T& rhs) { return !lhs.less_than(rhs); }
    };

    template <template <typename> class... CRTPs>
    struct Variadic : public CRTPs<Variadic<CRTPs...>>...
    {
        Variadic(int value = 0) : _value(value)
        {}

        bool equal_to(const Variadic& other) const
        {
            return _value == other._value;
        }

        bool less_than(const Variadic& other) const
        {
            return _value < other._value;
        }
        
    private:
        int _value;
    };
}

#endif /* CRTP_h */
