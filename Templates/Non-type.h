#ifndef Non_type_h
#define Non_type_h


/*
 Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
 C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
 C++17: можно писать auto для non-type template параметров.
 */

namespace non_type
{
    /// C++14:
    template<typename T, size_t N>
    struct array1
    {
    private:
        T data[N];
    };

    /// C++17:
    template<typename T, auto N>
    struct array2
    {
    private:
        T data[N];
    };

    template <auto val>
    struct Test
    {
        decltype(val) value;
    };

    /// C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения
    template<typename Type, Type n>
    inline constexpr void Print()
    {
        std::cout << typeid(n).name() << std::endl;
    }

    /// C++17: можно писать auto для non-type template параметров, вещественные числа передать нельзя
    template<auto... args>
    inline constexpr auto Sum()
    {
        return (args + ...);
    }

    template <int ...nums>
    void printNumbers ()  {
         // Раскрытие пакета для non-type template параметров в выражении свертки для печати чисел
         ((std::cout << nums << ' ' ), ...) ;
    }

    /// C++14: non-type template константа с неизвестным типом была передача двух параметров – типа и значения
    template <typename Type, Type value>
    constexpr Type TConstant1 = value;
    
    /// C++17: можно писать auto для non-type template констант, вещественные числа передать нельзя
    template <auto value>
    constexpr auto TConstant2 = value;

    template<typename T>
    struct Value
    {
        using type = T;
        constexpr Value(T&& iValue) // Вычисляется в compile time
        {
            value = iValue;
        }
        type value;
    };
    
    template<auto val, Value<decltype(val)> value> // В качестве шаблонного параметра объект
    auto Multiplication(auto iValue) // Сокращенный шаблон
    {
        return value.value * iValue;
    }
}

#endif /* Non_type_h */
