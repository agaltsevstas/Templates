#ifndef Non_type_h
#define Non_type_h


/*
 Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
 C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
 C++17: можно писать auto для non-type template параметров.
 */

namespace non_type
{
    template <typename T, size_t N>
    struct array
    {
    private:
        T data[N];
    };

    /// C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения
    template<typename Type, Type n>
    inline constexpr void print()
    {
        std::cout << typeid(n).name() << std::endl;
    }

    /// C++17: можно писать auto для non-type template параметров
    template <auto... args>
    inline constexpr auto sum()
    {
        return (args + ...);
    }
}

#endif /* Non_type_h */
