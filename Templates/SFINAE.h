#ifndef SFINAE_h
#define SFINAE_h

/*
* SFINAE (substitution failure is not an error) - при определении перегрузок функции ошибочные подстановки в шаблоны не вызывают ошибку компиляции, а отбрасываются из списка кандидатов на наиболее подходящую перегрузку.
  Правила:
  1. SFINAE это обязательно связано с перегрузкой функций.
  2. Это работает при автоматическом выводе типов шаблона (type deduction) по аргументам функции.
  3. Некоторые перегрузки могут отбрасываться в том случае, когда их невозможно определить из-за возникающей синтаксической ошибки; компиляция при этом продолжается, без ошибок.
  4. Отбросить могут только шаблон.
  5. SFINAE рассматривает только заголовок функции, ошибки в теле функции не будут пропущены.
*/

template <typename T>
struct Number
{
    Number(const T& number) : value(number) {}

    T value;
};

// SFINAE
namespace SFINAE
{
    /*
      При вызове функции компилятор должен выполнить проверку, чтобы узнать, какую из функций вызывать.
      Нужно учитывать перегрузку функций (function overloading), позволяющую давать нескольким функциям одинаковые имена.
      Шаги:
      1. Name lookup (поиск по имени);
      2. Template Argument Deduction (дедукция аргумента шаблона);
      3. Template Argument Substitution (замена аргумента шаблона);
      4. Overload Resolution (разрешение перегрузки).
    */

    /*
      Алгоритм SFINAE проявляется на шаге 3 (Template Argument Substitution), когда функция-кандидат проваливает тест Substitution (замены).
      Ошибка компиляции не возникает, а функция просто удаляется из списка кандидатов.
    */
    namespace
    {
        template<typename T>
        T Square(const T& number)
        {
            return number * number;
        }
    }

    /*
      Для Number<int> не работает: при вызове. Эта функция пытается найти int.value, которого не существует. Ошибку: часть else не удалена из функции (error: request for member ‘value’ in ‘t’, which is of non-class type ‘const int’).
    */
    namespace IS_ARITHMETIC
    {
        template<typename T>
        T Square(const T& number)
        {
            if (std::is_arithmetic<T>::value)
            {
                return number * number;
            }
            else
            {
                return number.value * number.value;
            }
        }
    }

    /*
    * Чтобы решить проблему Number<int>, нужны два шаблона функций, которые проверяют передаваемый тип на арифметическу.
    */
    namespace ENABLE_IF
    {
        template<typename T>
        typename std::enable_if<std::is_arithmetic<T>::value, T>::type Square(const T& number)
        {
            return number * number;
        }

        template<typename T>
        typename std::enable_if<!std::is_arithmetic<T>::value, T>::type Square(const T& number)
        {
            return number.value * number.value;
        }
    }

    /*
      Здесь используется только один шаблон функции (что упрощает код вместо std::enable_if).
      Компилятор берет только ветку с истинным условием (true) и отбрасывает другие.
    */
    namespace CONSTEXPR
    {
        template<typename T>
        T Square(const T& number)
        {
            if constexpr (std::is_arithmetic<T>::value) // Проверка нужна для класса Number, иначе ошибка: не найден operator*
            {
                return number * number;
            }
            else
            {
                return number.value * number.value;
            }
        }
    }
}

#endif /* SFINAE_h */
