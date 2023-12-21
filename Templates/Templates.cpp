#include <iostream>

#include "Arguments.h"
#include "Callback.h"
#include "Function.h"
#include "SFINAE.h"


int main()
{
    // Arguments
    {
        using namespace arguments;
        
        MeasureProperty property = MeasureProperty::Two;
        CheckProperty(property, MeasureProperty::One, MeasureProperty::Two, MeasureProperty::Three);
    }
    // callback
    {
        using namespace callback;
        
        Example example;
        Permission(example, &Example::Method).Run();
        Permission(example, &Example::Method1).Run();
        Permission(example, &Example::Method2).Run();
    }
    // function
    {
        using namespace function;
        
        std::shared_ptr<Base> pBase1 = std::make_shared<Derived1>();
        std::shared_ptr<Base> pBase2 = std::make_shared<Derived2>();
        std::shared_ptr<Derived1> derived1 = std::make_shared<Derived1>();
        std::shared_ptr<Derived2> derived2 = std::make_shared<Derived2>();
        std::unique_ptr<Derived1> pBase3 = std::make_unique<Derived1>();

        CallFunction1<Derived1>(pBase1);
        CallFunction1<Derived2>(pBase2);

        CallFunction1<std::unique_ptr<Base>>(pBase3);
        CallFunction1<std::shared_ptr<Base>>(derived2);

        CallFunction2<Derived1>(derived1);
        CallFunction2<Derived2>(derived2);
        CallFunction3(derived1);
        CallFunction3(derived2);
    }
    /*
    * SFINAE (substitution failure is not an error) - при определении перегрузок функции ошибочные подстановки в шаблоны не вызывают ошибку компиляции, а отбрасываются из списка кандидатов на наиболее подходящую перегрузку.
      Правила:
      1. SFINAE это обязательно связано с перегрузкой функций.
      2. Это работает при автоматическом выводе типов шаблона (type deduction) по аргументам функции.
      3. Некоторые перегрузки могут отбрасываться в том случае, когда их невозможно определить из-за возникающей синтаксической ошибки; компиляция при этом продолжается, без ошибок.
      4. Отбросить могут только шаблон.
      5. SFINAE рассматривает только заголовок функции, ошибки в теле функции не будут пропущены.
    */
    {
        int integer_num = 5;
        float floating_num = 5.0;
        bool boolean = true;
        Number<int> number_int(5);

        {
            auto square1 = SFINAE::Square(integer_num); // Вызов int Square(int);
            auto square2 = SFINAE::Square(floating_num); // Вызов float Square(float);
            auto square3 = SFINAE::Square(boolean);  // Вызов bool Square(bool);
            //auto square4 = SFINAE::Square(number_int); // Ошибка: не найден operator*
        }

        // std::is_arithmetic
        {
            //auto square1 = SFINAE::IS_ARITHMETIC::Square(integer_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
            //auto square2 = SFINAE::IS_ARITHMETIC::Square(floating_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
            //auto square3 = SFINAE::IS_ARITHMETIC::Square(boolean);  // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
            //auto square4 = SFINAE::IS_ARITHMETIC::Square(number_int); // Ошибка: бинарный "*": "const T" не определяет этот оператор или преобразование к типу приемлемо к встроенному оператору
        }

        // std::enable_if
        {
            auto square1 = SFINAE::ENABLE_IF::Square(integer_num); // Вызов int Square(int);
            auto square2 = SFINAE::ENABLE_IF::Square(floating_num); // Вызов float Square(float);
            auto square3 = SFINAE::ENABLE_IF::Square(boolean);  // Вызов bool Square(bool);
            auto square4 = SFINAE::ENABLE_IF::Square(number_int); // Вызов Number<int> Square(Number<int>);
        }

        // constexpr
        {
            auto square1 = SFINAE::CONSTEXPR::Square(integer_num); // Вызов int Square(int);
            auto square2 = SFINAE::CONSTEXPR::Square(floating_num); // Вызов float Square(float);
            auto square3 = SFINAE::CONSTEXPR::Square(boolean);  // Вызов bool Square(bool);
            auto square4 = SFINAE::CONSTEXPR::Square(number_int); // Вызов Number<int> Square(Number<int>);
        }
    }
    
    return 0;
}
