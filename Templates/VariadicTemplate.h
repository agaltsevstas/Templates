#ifndef Arguments_h
#define Arguments_h

/*
 Сайты: https://infotraining.bitbucket.io/cpp-adv/variadic-templates.html
 */


/*
 Variadic Template - шаблон с заранее неизвестным числом аргументов
 Оператор многоточия или элипсис  (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
 Например, std::tuple - может принять любое число входных параметров
 До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
 */

namespace variadic_template
{
    enum class MeasureProperty
    {
        One = 0,
        Two = 1,
        Three = 2
    };

    /// Stub-функция (заглушка) для рекурсии
    inline bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property)
    {
        return iProperty == Property;
    }

    template<typename... TArgs>
    inline bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property, TArgs&&... types)
    {
        return CheckProperty(iProperty, Property) || CheckProperty(iProperty, types...);
    }
    
    /// Stub-функция (заглушка) для рекурсии
    void print()
    {
        std::cout << "end print" << std::endl;
    }

    template<typename T, typename... TArgs>
    inline void print(const T& first, TArgs&&... args)
    {
        std::cout << first << std::endl;
        print(std::forward<TArgs>(args)...);
    }

    template<typename... Args>
    decltype(auto) Vector(Args&&... args)
    {
        std::vector<int> vec = {std::forward<Args>(args)...};
        return vec;
    }

    /* 
     Класс шаблона может иметь только один пакет параметров, и он должен быть помещен в конец списка параметров шаблона
     
     template<size_t... Indexes, typename... Ts> // error
     class Error;
     
     Обойти это ограничение можно следующим образом
     */
    template<size_t... Indexes> struct IndexSequence {};
    
    template<typename Indexes, typename ...Args>
    class Sequence
    {};

    /// Функции шаблона могут иметь больше пакетов параметров:
    template<int... Factors, typename... Args>
    void scale_and_print(Args&&... args)
    {
        print(Factors * args...);
    }

    template <typename... Mixins>
    class Mixin : public Mixins...
    {
    public:
        Mixin(Mixins&&... mixins) : Mixins(mixins)...
        {}
    };

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

#endif /* Arguments_h */
