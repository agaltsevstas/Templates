#ifndef Metafunction_h
#define Metafunction_h


/*
 Сайты: https://habr.com/ru/articles/337590/
        http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx

 Метафункция - это шаблонная (template) структура с constexpr членами, которые вычисляются во время компиляции.
 Стандартная библиотека (STL) придерживается соглашения, где в метафункциях указываются:
 Члены:
 - type - тип
 - value - возвращающееся значения, результат
 */


namespace metafunction
{
    template<class T, class U>
    struct is_same : std::false_type {};

    template<class T>
    struct is_same<T, T> : std::true_type {};

    template<int x>
    struct Square
    {
        static const int value = x * x;
    };

    /*
     Для факторила компилятор сгенерирует кол-во struct == числу, поэтому исполняемый файл (.exe) станет большим.
     */
    template<unsigned int N>
    struct Factorial
    {
        static constexpr unsigned int value = N * Factorial<N-1>::value;
    };

    template<>
    struct Factorial<0>
    {
        static constexpr unsigned int value = 1;
    };

    template<unsigned int N>
    struct Fibonacci
    {
        static constexpr unsigned int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
    };

    template<>
    struct Fibonacci<0>
    {
        static constexpr unsigned int value = 0;
    };

    template<>
    struct Fibonacci<1>
    {
        static constexpr unsigned int value = 1;
    };
}

#endif /* Metafunction_h */
