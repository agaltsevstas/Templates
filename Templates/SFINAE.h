#ifndef SFINAE_h
#define SFINAE_h

/// Сайты: https://stackoverflow.com/questions/74263416/question-about-has-const-iterator-has-begin-end
//        https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time

// Лекция: https://www.youtube.com/watch?v=v49lAJXnnPM&t=1s&ab_channel=ComputerScienceCenter

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
    
        // template<typename T>
        // typename std::enable_if<std::has_begin<T>::value, T>::type Square(const T& number)
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
    
        // TODO:
        template<auto Constant>
        void Convertible()
        {
            if constexpr (std::is_convertible_v<decltype(Constant), int>)
                std::cout << typeid(Constant).name() << " convertible to: int" << std::endl;
            else
                std::cout << "not convertible" << std::endl;
        }
    }

    namespace CONTAINER
    {
        struct Base
        {
            // Вспомогательные типы: yes и no, чтобы различать их по размеру
            using yes = char[2];
            using no = char[1];
        };

        template<typename T>
        class is_class : private Base
        {
            template<typename C>
            static yes& test(int C::*); // вернет true - это класс, иначе false
         
            template<typename C>
            static no& test(...); // Элипсис (...) при перегрузке идет в приоритете в конце
        public:
            static bool const value = sizeof(test<T>(nullptr)) == sizeof(yes);
        };

        template<typename T>
        struct has_const_iterator : private Base
        {
        private:
            template<typename C>
            static yes& test(typename C::const_iterator*); // вернет true - это контейнер с итераторами, иначе false
            template<typename C>
            static no& test(...); // Элипсис (...) при перегрузке идет в приоритете в конце
        public:
          static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
          using type = T;
        };

        template<typename T>
        struct has_begin_end : private Base
        {
        private:
            template<typename C>
            static yes& f(typename std::enable_if<std::is_same<decltype(static_cast<typename C::const_iterator(C::*)()const>(&C::begin)), // вернет true - это контейнер с итераторами, иначе false
            typename C::const_iterator(C::*)() const>::value>::type*);

            template<typename C>
            static no& f(...);

            template<typename C>
            static yes& g(typename std::enable_if<std::is_same<decltype(static_cast<typename C::const_iterator(C::*)()const>(&C::end)), // вернет true - это контейнер с итераторами, иначе false
            typename C::const_iterator(C::*)() const>::value, void>::type*);

            template<typename C>
            static no& g(...);

        public:
          static bool const begin_value = sizeof(f<T>(nullptr)) == sizeof(yes);
          static bool const end_value = sizeof(g<T>(nullptr)) == sizeof(yes);
        };

        template<typename T>
        struct is_container
        {
          static const bool value = has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value;
        };
    
        namespace ENABLE_IF
        {
            template <typename T>
            typename std::enable_if<has_const_iterator<T>::value, void>::type
            Print(const T& container)
            {
                for (const auto& item : container)
                    std::cout << item << ", ";
                std::cout << std::endl;
            }
        
            template <typename T>
            typename std::enable_if<!has_const_iterator<T>::value, void>::type
            Print(const T& value)
            {
                std::cout << value << std::endl;
            }
        }
        namespace CONSTEXPR
        {
            template<typename T>
            void Print(const T& values)
            {
                if constexpr (has_const_iterator<T>::value) // Проверка нужна для контейнеров, иначе обычное значение
                {
                    for (const auto& value : values)
                        std::cout << value << ", ";
                    std::cout << std::endl;
                }
                else
                {
                    std::cout << values << std::endl;
                }
            }
        }
    }
}


#endif /* SFINAE_h */
