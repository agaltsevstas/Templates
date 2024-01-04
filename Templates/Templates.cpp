#include <iostream>

#include "VariadicTemplate.h"
#include "Callback.h"
#include "Instantiation.h"
#include "CRTP.h"
#include "FoldExpression.h"
#include "Function.h"
#include "Non-type.h"
#include "SFINAE.h"
#include "Specialization.h"
#include "typedef_using.h"

/*
 Сайты: http://scrutator.me/post/2017/08/11/cpp17_lang_features_p1.aspx
        http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx
        https://cpp-kt.github.io/cpp-notes/14_templates.html
        https://iamsorush.com/posts/cpp-variadic-template/
        https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file/495056#495056
        https://www.scs.stanford.edu/~dm/blog/param-pack.html
        https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used
        https://gist.github.com/sergeysablin99/1eb61b51fb58d5ae0a58ac1424c249c6
 extern template : https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
 */


/*
 Шаблон (template) — паттерн который создает классы или функции различных типов.
 
 Инстанцирование шаблона – это генерация кода функции или класса через подстановку параметров в шаблон.
 При инстанцировании 3 разных типов: int, double, string, компилятор создает три разные функции/классы.
 2 Вида инстанцирования:
 1. Неявное инстанцирование (implicit instantiation) - происходит при вызове функции или создании объекта класса (обычно в .h файлах)
 Плюсы:
 - Не нужно явно инстанцировать все типы в .cpp файле
 - Не нужно явно инстанцировать все типы, если шаблон инициализирован и определен в одном .cpp файле в том месте, где он вызывается
 Минусы:
 - Размещение шаблонов в .h файлах может привести к загромождению размера кода и размера скомпилированного двоичного файла.
 2. Явное инстанцирование (explicit instantiatian) с помощью резервированного слова template (обычно в .cpp файлах). Каждая единица трансляции транслируется отдельно, а потом всё линкуется. Инстанцирование шаблонов (подстановка) происходит до линковки. Компилятор при генерировании кода должен видеть определение шаблона (а не только объявление), так и определения типов, которые подставляются в шаблон. Поэтому если класс/функция инициализированы в .h файле, а реализация в .cpp файле, то при одновременной комплияции двух .cpp файлов компилятор не запомнит подставляемый тип в одном .cpp файла, где вызывается шаблон, во время компиляции другого .cpp файла, где находится реализация метода/класса. В .cpp файле, где реализация метода/класса нужно сделать явное инстанцирование.
 Плюсы:
   - Сокрытие явной реализации класса/метода в .cpp файле
   - Явное создание экземпляров позволяет сократить время компиляции
   - Предотвращение переопределение объекта (экономит время и размер)
 Минусы:
 - Если не создать явное инстанцирование всех типов в .cpp файле, то будет ошибка: undefined reference.
 Дополнение: Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки. Можно указать компилятору не инстанцировать шаблон в данной единице трансляции, для этого перед инстанцированием указать extern - это уменьшает время компиляции.
 
 Компилятор в приоритете выбирает более специлизированный шаблон
 Специализации шаблонов по приоритетам:
 1. Полная специализация (explicit (или full) specialization) - напоминает наследование или перегрузку - template<> class vector<bool>, тогда компилятор не будет дописывать класс под конкретный тип данных, а вызовет уже объявленную специализацию, таким образом мы может запретить для какого-то типа определенные действия или дополнить, в отличие от остальных типов
 2. Частичная специализация (partial specialization) - только для классов template<Class A> class vector<bool, A>.
 Для методов нельзя, но это можно обойти сделать в частичном специализированном классе static метод.
 3. Основной шаблон (primary template) - vector<T>
 
 Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
 C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
 C++17: можно писать auto для non-type template параметров.
 
 Variadic Template - шаблон с заранее неизвестным числом аргументов
 Оператор многоточия или элипсис  (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
 Например, std::tuple - может принять любое число входных параметров
 До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
 
 Fold expression (выражение свертки) - шаблон с заранее неизвестным числом аргументов (variadic template). Свертка – это функция, которая применяет заданную комбинирующую функцию к последовательным парам элементов в списке и возвращает результат. Любое выражение свёртки должно быть заключено в скобки и в общем виде выглядит так: (выражение содержащее пачку аргументов). Выражение внутри скобок должно содержать в себе нераскрытую пачку параметров и один из следующих операторов:
 +  -  *  /  %  ^  &  |  =  <  >  <<  >>
 +=  -=  *=  /=  %=  ^=  &=  |=  <<=  >>=
 ==  !=  <=  >=  &&  ||  ,  .*  ->*
 
 До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
 
 Template argument deduction - способность шаблоннов определять тип передаваемых аргументов без явного указания типа: вместо foo<...>(...) можно foo(...). До C++17 при инстанцировании шаблона функции нужно было явно указывать типы аргументы: foo<...>(...)
 */

void func(int a, int b, int c)
{
}

// Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки. Можно указать компилятору не инстанцировать шаблон в данной единице трансляции, для этого перед инстанцированием указать extern - это уменьшает время компиляции.
extern template int instantiation::Instantiation::explicitCountArgs(const int);
extern template int instantiation::Instantiation::explicitCountArgs(const double);
extern template int instantiation::Instantiation::explicitCountArgs(const std::string);
extern template int instantiation::Instantiation::explicitCountArgs(const int, const double, const std::string);

int main()
{
    /*
     Инстанцирование шаблона – это генерация кода функции или класса через подстановку параметров в шаблон.
     При инстанцировании 3 разных типов: int, double, string, компилятор создает три разные функции/классы.
     2 Вида инстанцирования:
     1. Неявное инстанцирование (implicit instantiation) - происходит при вызове функции или создании объекта класса (обычно в .h файлах)
     Плюсы:
     - Не нужно явно инстанцировать все типы в .cpp файле
     - Не нужно явно инстанцировать все типы, если шаблон инициализирован и определен в одном .cpp файле в том месте, где он вызывается
     Минусы:
     - Размещение шаблонов в .h файлах может привести к загромождению размера кода и размера скомпилированного двоичного файла.
     2. Явное инстанцирование (explicit instantiatian) с помощью резервированного слова template (обычно в .cpp файлах). Каждая единица трансляции транслируется отдельно, а потом всё линкуется. Инстанцирование шаблонов (подстановка) происходит до линковки. Компилятор при генерировании кода должен видеть определение шаблона (а не только объявление), так и определения типов, которые подставляются в шаблон. Поэтому если класс/функция инициализированы в .h файле, а реализация в .cpp файле, то при одновременной комплияции двух .cpp файлов компилятор не запомнит подставляемый тип в одном .cpp файла, где вызывается шаблон, во время компиляции другого .cpp файла, где находится реализация метода/класса. В .cpp файле, где реализация метода/класса нужно сделать явное инстанцирование.
     Плюсы:
       - Сокрытие явной реализации класса/метода в .cpp файле
       - Явное создание экземпляров позволяет сократить время компиляции
       - Предотвращение переопределение объекта (экономит время и размер)
     Минусы:
     - Если не создать явное инстанцирование всех типов в .cpp файле, то будет ошибка: undefined reference.
    */
    {
        using namespace instantiation;
        
        Instantiation instance;
        auto implicitCountArguments1 = instance.implicitCountArgs(int(1));
        auto implicitCountArguments2 = instance.implicitCountArgs(double(1.0));
        auto implicitCountArguments3 = instance.implicitCountArgs(std::string("1.0"));
        auto implicitCountArguments4 = instance.implicitCountArgs(int(1), double(1.0), std::string("1.0"));
        
        auto explicitCountArguments1 = instance.explicitCountArgs(int(1));
        auto explicitCountArguments2 = instance.explicitCountArgs(double(1.0));
        auto explicitCountArguments3 = instance.explicitCountArgs(std::string("1.0"));
        auto explicitCountArguments4 = instance.explicitCountArgs(int(1), double(1.0), std::string("1.0"));
    }
    /*
     Компилятор в приоритете выбирает более специлизированный шаблон
     Специализации шаблонов по приоритетам:
     1. Полная специализация (explicit (или full) specialization) - напоминает наследование или перегрузку - template<> class vector<bool>, тогда компилятор не будет дописывать класс под конкретный тип данных, а вызовет уже объявленную специализацию, таким образом мы может запретить для какого-то типа определенные действия или дополнить, в отличие от остальных типов
     2. Частичная специализация (partial specialization) - только для классов template<Class A> class vector<bool, A>.
     Для методов нельзя, но это можно обойти сделать в частичном специализированном классе static метод.
     3. Основной шаблон (primary template) - vector<T>
     */
    {
        using namespace specialization;
        
        Person<std::string, std::string> nick("Nick", "20"); // Основной шаблон (primary template)
        Person<std::string, int> alex("Alex", 21); // Основной шаблон (primary template)
        Person<const char*, unsigned> maria("Maria", 25); // Частичная специализация (partial specialization)
        Person<std::string_view, unsigned> Dan("Dan", 28); // Частичная специализация (partial specialization)
        Person<std::string, unsigned> stas("Stas", 23); // Полная специализация (explicit (или full) specialization)
    }
    /*
     Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
     C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
     C++17: можно писать auto для non-type template параметров.
     */
    {
        using namespace non_type;
        
        array<int, 10> a;
        
        print<int, 10>(); // C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения
        auto sum_result2 = sum<1, 2, 3>(); // C++17: можно писать auto для non-type template параметров
    }
    /*
     Variadic Template - шаблон с заранее неизвестным числом аргументов
     Оператор многоточия или элипсис  (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
     Например, std::tuple - может принять любое число входных параметров
     До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
     */
    {
        using namespace variadic_template;
        
        MeasureProperty property = MeasureProperty::Two;
        CheckProperty(property, MeasureProperty::One, MeasureProperty::Two, MeasureProperty::Three);
        print("one", std::string{"two"}, 3, 4.0);
    }
    /*
     Fold expression (выражение свертки) - шаблон с заранее неизвестным числом аргументов (variadic template). Свертка – это функция, которая применяет заданную комбинирующую функцию к последовательным парам элементов в списке и возвращает результат. Любое выражение свёртки должно быть заключено в скобки и в общем виде выглядит так: (выражение содержащее пачку аргументов). Выражение внутри скобок должно содержать в себе нераскрытую пачку параметров и один из следующих операторов:
     +  -  *  /  %  ^  &  |  =  <  >  <<  >>
     +=  -=  *=  /=  %=  ^=  &=  |=  <<=  >>=
     ==  !=  <=  >=  &&  ||  ,  .*  ->*
     
     До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
     */
    {
        using namespace fold_expression;
        std::vector<int> numbers;
        
        auto sum_result1 = sum(1, 2, 3);
        auto average_result = average(1, 2, 3);
        auto norm_result = norm(1, 2, 3);
        auto pow_sum_result = pow_sum(1, 2, 3);
        push_to_vector(numbers, 1, 2, 3, 4, 5);
        auto countArguments = countArgs(1, "hello", 2.f);
        auto countArgumentsFunction = countArgsFunction(func).value;
        checkTypes(int(1), std::string("hello"), double(2.0));
        print_strings("one", std::string{"two"});
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
     SFINAE (substitution failure is not an error) - при определении перегрузок функции ошибочные подстановки в шаблоны не вызывают ошибку компиляции, а отбрасываются из списка кандидатов на наиболее подходящую перегрузку.
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

        using namespace SFINAE;
        // Square
        {
            {
                auto square1 = Square(integer_num); // Вызов int Square(int);
                auto square2 = Square(floating_num); // Вызов float Square(float);
                auto square3 = Square(boolean);  // Вызов bool Square(bool);
                //auto square4 = SFINAE::Square(number_int); // Ошибка: не найден operator*
            }

            // std::is_arithmetic
            {
                //auto square1 = IS_ARITHMETIC::Square(integer_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square2 = IS_ARITHMETIC::Square(floating_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square3 = IS_ARITHMETIC::Square(boolean);  // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square4 = IS_ARITHMETIC::Square(number_int); // Ошибка: бинарный "*": "const T" не определяет этот оператор или преобразование к типу приемлемо к встроенному оператору
            }

            // std::enable_if
            {
                using namespace SFINAE;
                
                auto square1 = ENABLE_IF::Square(integer_num); // Вызов int Square(int);
                auto square2 = ENABLE_IF::Square(floating_num); // Вызов float Square(float);
                auto square3 = ENABLE_IF::Square(boolean);  // Вызов bool Square(bool);
                auto square4 = ENABLE_IF::Square(number_int); // Вызов Number<int> Square(Number<int>);
            }

            // constexpr
            {
                auto square1 = CONSTEXPR::Square(integer_num); // Вызов int Square(int);
                auto square2 = CONSTEXPR::Square(floating_num); // Вызов float Square(float);
                auto square3 = CONSTEXPR::Square(boolean);  // Вызов bool Square(bool);
                auto square4 = CONSTEXPR::Square(number_int); // Вызов Number<int> Square(Number<int>);
                
                // TODO:
                CONSTEXPR::convertible<1>();
            }
        }
        
        // CONTAINER
        {
            using namespace CONTAINER;
            constexpr const int number = 10;
            constexpr std::string_view value = "10";
            std::vector<int> numbers { 1,2,3,4,5 };
            auto has_const_iterator1 = has_const_iterator<std::vector<int>>::value;
            auto has_const_iterator2 = has_const_iterator<int>::value;
            
            auto has_begin_value1 = has_begin_end<std::vector<int>>::begin_value;
            auto has_end_value1 = has_begin_end<std::vector<int>>::end_value;
            auto has_begin_value2 = has_begin_end<int>::end_value;
            auto has_end_value2 = has_begin_end<int>::begin_value;
            
            // std::enable_if
            {
                CONTAINER::ENABLE_IF::Print(numbers);
                CONTAINER::ENABLE_IF::Print(number);
                CONTAINER::ENABLE_IF::Print(value);
            }
            // constexpr
            {
                CONTAINER::CONSTEXPR::Print(numbers);
                CONTAINER::CONSTEXPR::Print(number);
                CONTAINER::CONSTEXPR::Print(value);
            }
            
            // CONCEPTS
            {
                CONTAINER::CONCEPTS::Print(numbers);
            }
        }
    }
    /*
     CRTP (curiously recurring template pattern) - рекурсивный шаблон: класс Derived наследуется от шаблона класса Base, использующего Derived как шаблонный параметр. Вызов Base::interface вызывает Derived::interface. Используется статический полиморфизм вместо динамического полиморфизма (без виртуальных функций)
     Плюсы:
     - делается во время компиляции (compile), а не во время исполнения (runtime)
     - нет накладных расходов на создание таблиц виртуальных функций
     - время, затрачиваемое на выбор метода
     Минусы:
     - У всех классов-наследников должны определены методы, которые которые вызываются в базовом классе
    */
    {
        CRTP::Derived derived;
        derived.interface1();
        derived.interface2();
    }
    
    return 0;
}
