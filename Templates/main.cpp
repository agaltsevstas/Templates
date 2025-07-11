#include <iostream>

#include "Auto.h"
#include "Callback.h"
#include "Forward.h"
#include "Instantiation.h"
#include "invoke_apply.h"
#include "Concept.h"
#include "CRTP.h"
#include "FoldExpression.h"
#include "Function.h"
#include "Non-type.h"
#include "Matching.h"
#include "Metafunction.h"
#include "SFINAE.h"
#include "Specialization.h"
#include "typedef_using.h"
#include "Tuple.h"
#include "VariadicTemplate.h"

#include <array>
#include <list>


/*
 Сайты: http://scrutator.me/post/2017/08/11/cpp17_lang_features_p1.aspx
        http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx
        https://cpp-kt.github.io/cpp-notes/14_templates.html
        https://iamsorush.com/posts/cpp-variadic-template/
        https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file/495056#495056
        https://www.scs.stanford.edu/~dm/blog/param-pack.html
        https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used
        https://gist.github.com/sergeysablin99/1eb61b51fb58d5ae0a58ac1424c249c6
 CRTP, variadic: https://infotraining.bitbucket.io/cpp-adv/variadic-templates.html
                 https://www.fluentcpp.com/2018/06/22/variadic-crtp-opt-in-for-class-features-at-compile-time/
 extern template : https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
 invoke, apply: http://scrutator.me/post/2018/05/25/cpp17_lang_features_p4.aspx
                https://habr.com/ru/companies/pvs-studio/articles/340014/
                https://habr.com/ru/companies/otus/articles/656363/
                https://stackoverflow.com/questions/38222029/variadic-members-in-non-template-class?rq=3
                https://www.vishalchovatiya.com/variadic-template-cpp-implementing-unsophisticated-tuple/
                https://www.itcodar.com/c-plus-1/template-tuple-calling-a-function-on-each-element.html
                https://medium.com/@raghavmnnit/variadic-templates-and-function-arguments-part2-1d35d06730d9
 instantiation: Лекция (время 44:30): https://www.youtube.com/watch?v=G_jcBrrYPAs&ysclid=mct1fzo0rj301368439
 metafunction: https://habr.com/ru/articles/337590/
               http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx
 matching: https://habr.com/ru/articles/282630/
 SFINAE: Сайты: https://stackoverflow.com/questions/74263416/question-about-has-const-iterator-has-begin-end
                https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time
         Лекция: https://www.youtube.com/watch?v=v49lAJXnnPM&t=1s&ab_channel=ComputerScienceCenter
 tuple: https://www.itcodar.com/c-plus-1/template-tuple-calling-a-function-on-each-element.html
        https://infotraining.bitbucket.io/cpp-adv/variadic-templates.html
        https://www.scs.stanford.edu/~dm/blog/param-pack.html
        https://github.com/jfalcou/kumi/blob/main/include/kumi/tuple.hpp
        https://github.com/glebzlat/tuple/blob/main/include/Tuple.hpp
        https://github.com/deepgrace/smp/blob/master/include/fuple.hpp
        https://github.com/VladimirBalun/Metaprogramming/blob/master/tuple.hpp
        https://chromium.googlesource.com/external/github.com/domokit/base/+/0032c8e1a72eb85d947d8df8de503caa62b4d0a8/tuple.h
        https://mitchnull.blogspot.com/2012/06/c11-tuple-implementation-details-part-1.html
        https://www.vishalchovatiya.com/variadic-template-cpp-implementing-unsophisticated-tuple/
        https://stackoverflow.com/questions/4041447/how-is-stdtuple-implemented
        https://gist.github.com/seiren-naru-shirayuri/d0493137d688eb91277d00e0ed48b9a2
        https://gist.github.com/ericniebler/fa621a311acd2f0339c57e01824c654c
        https://gist.github.com/IvanVergiliev/9639530
 */


/*
 Шаблон (template) — паттерн который создает классы или функции различных типов.
 
 Инстанцирование шаблона – это генерация кода функции или класса через подстановку параметров в шаблон.
 При инстанцировании 3 разных типов: int, double, string, компилятор создает три разные функции/классы.
 2 Вида инстанцирования:
 1. Неявное инстанцирование (implicit instantiation) - происходит при вызове функции или создании объекта класса (обычно в .h файлах)
 Плюсы:
 - не нужно явно инстанцировать все типы в .cpp файле
 - не нужно явно инстанцировать все типы, если шаблон инициализирован и определен в одном .cpp файле в том месте, где он вызывается
 Минусы:
 - Размещение шаблонов в .h файлах может привести к загромождению размера кода и размера скомпилированного двоичного файла.
 2. Явное инстанцирование (explicit instantiation) с помощью резервированного слова template (обычно в .cpp файлах). Каждая единица трансляции транслируется отдельно, а потом всё линкуется. Инстанцирование шаблонов (подстановка) происходит до линковки. Компилятор при генерировании кода должен видеть определение шаблона (а не только объявление), так и определения типов, которые подставляются в шаблон. Поэтому если класс/функция инициализированы в .h файле, а реализация в .cpp файле, то при одновременной комплияции двух .cpp файлов компилятор не запомнит подставляемый тип в одном .cpp файла, где вызывается шаблон, во время компиляции другого .cpp файла, где находится реализация метода/класса. В .cpp файле, где реализация метода/класса нужно сделать явное инстанцирование.
 Плюсы:
   - Сокрытие явной реализации класса/метода в .cpp файле
   - Предотвращение переопределение объекта (экономит время и размер)
 Минусы:
 - Если не создать явное инстанцирование всех типов в .cpp файле, то будет ошибка: undefined reference
 - Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки
 Дополнение: Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки. Можно указать явное объявление инстанциации с помощью extern в .h файле и одно определение в .cpp файле, и тогда компилятору не нужно будет инстанцировать шаблон в каждой единице трансляции, что уменьшает время компиляции.
 
 Компилятор в приоритете выбирает более специлизированный шаблон
 Специализации шаблонов по приоритетам:
 1. Полная специализация (explicit (или full) specialization) - напоминает наследование или перегрузку - template<> class vector<bool>, тогда компилятор не будет дописывать класс под конкретный тип данных, а вызовет уже объявленную специализацию, таким образом мы может запретить для какого-то типа определенные действия или дополнить, в отличие от остальных типов
 2. Частичная специализация (partial specialization) - только для классов template<Class A> class vector<bool, A>.
 Для методов нельзя, но это можно обойти сделать в частичном специализированном классе static метод.
 3. Основной шаблон (primary template) - vector<T>
 
 4 типа шаблонов:
 - шаблоны функций: template<typename T> T max(T &a, T &b) { return a > b ? a : b; }
 - шаблоны классов: template<class T> class { T t; };
 - шаблоны переменных: template<typename T> constexpr T pi = T{3.141592653589793238462643383L};
 - Non-type параметры: template<class T, size_t N> class array. Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции. C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения. C++17: можно писать auto для non-type template параметров.
 
 Variadic Template - шаблон с заранее неизвестным числом аргументов.
 Оператор многоточия или элипсис (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
 Например, std::tuple - может принять любое число входных параметров.
 До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка).
 
 Fold expression (выражение свертки) - шаблон с заранее неизвестным числом аргументов (variadic template). Свертка – это функция, которая применяет заданную комбинирующую функцию к последовательным парам элементов в списке и возвращает результат. Любое выражение свёртки должно быть заключено в скобки и в общем виде выглядит так: (выражение содержащее пачку аргументов). Выражение внутри скобок должно содержать в себе нераскрытую пачку параметров и один из следующих операторов:
 +  -  *  /  %  ^  &  |  =  <  >  <<  >>
 +=  -=  *=  /=  %=  ^=  &=  |=  <<=  >>=
 ==  !=  <=  >=  &&  ||  ,  .*  ->*
 
 CTAD (Сlass template argument deduction) - способность шаблоннов определять тип передаваемых аргументов без явного указания типа: вместо foo<...>(...) можно foo(...). До C++17 при инстанцировании шаблона функции нужно было явно указывать типы аргументы: foo<...>(...).
 
 Сокращенный шаблон (auto или Concept auto) - шаблонная функция, которая содержит auto в качестве типа аргумента или возвращающегося значения.
 Плюсы:
 - упрощает синтаксис
 - не нужно писать template
 Минусы:
 - если забыть сделать return в функции, то функция станет void
 - можно вернуть не тот тип
 Решение: использовать концепты для точного возвращения нужного типа
 
 Non-type template parameter (шаблонные аргументы не являющиеся типами) - шаблонные аргументы-константы, в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
 C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
 C++17: можно писать auto для non-type template параметров.
 С++20: можно работать с вещественными числами.
 
 std::forward - идеальной передача (perfect forwarding).
 Свойства:
 - std::forward РАБОТАЕТ с T&& с НЕконстантными ссылками
 - std::forward НЕ работает с const T& - для всех типов ссылок (rvalue, const rvalue, lvalue, const lvalue), т.к. rvalue(T&&) -> lvalue (T&) на основе сжатии ссылок (T&(A&&) = A& // && отбрасывается).
 - std::forward НЕ будет работает c const&& для lvalue

 std::forward используется ТОЛЬКО в шаблонах:
 template<typename T> T&& forward(T&& param)
 {
     if (is_lvalue_reference<T>::value)
         return param;
     else
         return move(param);
 }
 Тип typename T имеет 2 перегрузки:
 1. lvalue-ссылка (T = const A& или T = A&)
 2. rvalue-ссылка (T = A&&)
 3. Бессылочный тип (T = A*) - указатель
 Идеальная передача позволяет создавать функции-обертки, где lvalue-выражения копируются (просто возвращается lvalue), а rvalue-выражения перемещаются (std::move).
 Использование:
 class Example
 {
    std::string field;
    template <class T>
    Example(T&& x) : field(std::forward<T>(x)) {}
 };
 Отличие std::forward от std::move: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
 
 reference collapse — сжатие ссылок, которое используется только в шаблонах и определяет поведение (отбрасывает не нужные &) при появлении ссылки на ссылку. Меньшее число (&) - выигрывает у большего числа (&&).
 Пример:
 T инстанцирован A&&, то T&& (A&& &&) = A&& // && отбрасывается
 Правила:
 T&(A&)  = A& // & отбрасывается
 T&&(A&) = A& // && отбрасывается
 T&(A&&) = A& // && отбрасывается
 T&&(A&&) = A&& // && отбрасывается, но тип xvalue->lvalue
 Универсальной ссылка - аргумент типа T&&, где T — шаблонный тип, может принимать оба типа ссылок (A& и A&&).
 Пример:
 template<typename T> function(T&& arg) // arg - lvalue всегда, т.к. T&& arg - xvalue, поэтому будет вызываться конструктор копирования
 */

namespace fold_expression
{
    void Func(int a, int b, int c)
    {
    }
}

/* Lambda можно передавать шаблоны только при аргументах */
namespace LAMBDA
{
    void MyFunction(int&& number)
    {
         std::cout << "rvalue: " << number << std::endl;
    }
    void MyFunction(int& number)
    {
        std::cout << "lvalue: " << number << std::endl;
    }
}

// Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки. Можно указать компилятору не инстанцировать шаблон в данной единице трансляции, для этого перед инстанцированием указать extern - это уменьшает время компиляции.
extern template int instantiation::Instantiation::ExplicitCountArgs(int&);
extern template int instantiation::Instantiation::ExplicitCountArgs(int&&);
extern template int instantiation::Instantiation::ExplicitCountArgs(double&);
extern template int instantiation::Instantiation::ExplicitCountArgs(double&&);
extern template int instantiation::Instantiation::ExplicitCountArgs(std::string&);
extern template int instantiation::Instantiation::ExplicitCountArgs(std::string&&);
extern template int instantiation::Instantiation::ExplicitCountArgs(int&, double&, std::string&);
extern template int instantiation::Instantiation::ExplicitCountArgs(int&&, double&&, std::string&&);

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
        std::cout << "instantiation" << std::endl;
        
        Instantiation instance;
        [[maybe_unused]] auto implicitCountArguments1 = instance.ImplicitCountArgs(1);
        [[maybe_unused]] auto implicitCountArguments2 = instance.ImplicitCountArgs(1.0);
        [[maybe_unused]] auto implicitCountArguments3 = instance.ImplicitCountArgs("1.0");
        [[maybe_unused]] auto implicitCountArguments4 = instance.ImplicitCountArgs(1, 1.0, "1.0");
        
        //auto explicitCountArguments1 = instance.explicitCountArgs(1);
        //auto explicitCountArguments2 = instance.explicitCountArgs(1.0);
        //auto explicitCountArguments3 = instance.explicitCountArgs<std::string>("1.0");
        //auto explicitCountArguments4 = instance.explicitCountArgs<int, double, std::string>(1, 1.0, "1.0");
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
        std::cout << "specialization" << std::endl;
        
        Person<std::string, std::string> nick("Nick", "20"); // Основной шаблон (primary template)
        Person<std::string, int> alex("Alex", 21); // Основной шаблон (primary template)
        Person<const char*, unsigned> maria("Maria", 25); // Частичная специализация (partial specialization)
        Person<std::string_view, unsigned> Dan("Dan", 28); // Частичная специализация (partial specialization)
        Person<std::string, unsigned> stas("Stas", 23); // Полная специализация (explicit (или full) specialization)
    }
    /*
     Сокращенный шаблон (auto или Concept auto) - шаблонная функция, которая содержит auto в качестве типа аргумента или возвращающегося значения
     Плюсы:
     - упрощает синтаксис
     - не нужно писать template
     Минусы
     - если забыть сделать return в функции, то функция станет void
     - можно вернуть не тот тип
     Решение: использовать концепты для точного возвращения нужного типа
    */
    {
        [[maybe_unused]] auto sum1 = AUTO::GetSum(1, 2);
        [[maybe_unused]] auto sum2 = AUTO::GetSum(1, 2.f);
        [[maybe_unused]] auto sum3 = AUTO::Sum(1, 2);
        [[maybe_unused]] auto sum4 = AUTO::Sum(1, 2.f);

        // Concept
        {
            std::vector<int> points { 1, 2, 3, 4, 5 };
            CONCEPT::Point point;

            AUTO::CONCEPT::Print_Strings("one", std::string{ "two" });
            AUTO::CONCEPT::Print(points);
            
            // auto resultReturn = CONCEPT::AUTO::NothingReturn(); // Ошибка NothingReturn ничего не возвращает
            CONCEPT::AUTO::DrawShape(point);
            [[maybe_unused]] auto shape1 = CONCEPT::AUTO::GetShape1(); // C++20, 1 Способ: точно возвращается значение и нужный тип
            [[maybe_unused]] auto shape2 = CONCEPT::AUTO::GetShape2(); // C++20, 2 Способ: точно возвращается значение и нужный тип
        }
    }
    /*
     Non-type template parameter (шаблонные аргументы не являющиеся типами) - шаблонные аргументы-константы, в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции.
     C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения.
     C++17: можно писать auto для non-type template параметров.
     С++20: можно работать с вещественными числами
     */
    {
        using namespace non_type;
        std::cout << "non type" << std::endl;
        
        [[maybe_unused]] array1<int, 10> a1;
        [[maybe_unused]] array2<int, 10> a2;
        
        [[maybe_unused]] Test<10> test1;
        // Test<10.5> test2; // Ошибка в XCode
        
        TConstant1<int, 10>;
        TConstant2<10>;
        
        Print<int, 10>(); // C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения
        
        // С++20: вещественные числа
        [[maybe_unused]] auto constexpr sum_result2 = Sum<1, 2, 3>(); // C++17: можно писать auto для non-type template параметров, вещественные числа передать нельзя
        
        [[maybe_unused]] constexpr Value<int> v1(2); // Без constexpr будет ошибка
        [[maybe_unused]] constexpr Value<double> v2(2.5); // Без constexpr будет ошибка
        [[maybe_unused]] auto multi1 = Multiplication<v1.value, v1>(100);
        // auto multi2 = Multiplication<v2.value, v2>(100.0); // Ошибка в XCode
    }
    /*
     std::forward - идеальной передача (perfect forwarding).
     Свойства:
     - std::forward РАБОТАЕТ с T&& с НЕконстантными ссылками
     - std::forward НЕ работает с const T& - для всех типов ссылок (rvalue, const rvalue, lvalue, const lvalue), т.к. rvalue(T&&) -> lvalue (T&) на основе сжатии ссылок (T&(A&&) = A& // && отбрасывается).
     - std::forward НЕ будет работает c const&& для lvalue

     std::forward используется ТОЛЬКО в шаблонах:
     template<typename T> T&& forward(T&& param)
     {
         if (is_lvalue_reference<T>::value)
             return param;
         else
             return move(param);
     }
     Тип typename T имеет 2 перегрузки:
     1. lvalue-ссылка (T = const A& или T = A&)
     2. rvalue-ссылка (T = A&&)
     3. Бессылочный тип (T = A*) - указатель

     Идеальная передача позволяет создавать функции-обертки, где lvalue-выражения копируются (просто возвращается lvalue), а rvalue-выражения перемещаются (std::move).
     Использование:
     class Example
     {
        std::string field;
        template <class T>
        Example(T&& x) : field(std::forward<T>(x)) {}
     };
     
     reference collapse — сжатие ссылок, которое используется только в шаблонах и определяет поведение (отбрасывает не нужные &) при появлении ссылки на ссылку. Меньшее число (&) - выигрывает у большего числа (&&).
     Пример:
     T инстанцирован A&&, то T&& (A&& &&) = A&& // && отбрасывается
     Правила:
     T&(A&)  = A& // & отбрасывается
     T&&(A&) = A& // && отбрасывается
     T&(A&&) = A& // && отбрасывается
     T&&(A&&) = A&& // && отбрасывается, но тип xvalue->lvalue
     Универсальной ссылка - аргумент типа T&&, где T — шаблонный тип, может принимать оба типа ссылок (A& и A&&).
     Пример:
     template<typename T> function(T&& arg) // arg - lvalue всегда, т.к. T&& arg - xvalue, поэтому будет вызываться конструктор копирования
     Отличие std::forward от std::move: std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).
     */
    {
        using namespace FORWARD;
        std::cout << "FORWARD" << std::endl;
        
        int number = 1;
        Function(number); // lvalue: T - int&, arg - int&, std::cout << "&" << "&&" << "&" << std::endl;
        Function(0); // rvalue: T - int, arg - int&&, std::cout << "&" << "&&" << "&&" << std::endl;
        std::cout << "--------------------" << std::endl;
        
        A a;
        std::cout << "--------------------" << std::endl;
        // Без std::forward
        {
            std::cout << "WITHOUT FORWARD" << std::endl;
            auto shared_ptr1 = Make_Shared<A>(a);   // Должно произойти копирование
            auto shared_ptr2 = Make_Shared<A>(A()); // Должно произойти копирование
            std::cout << "--------------------" << std::endl;
        }
        
        // std::forward
        {
            std::cout << "FORWARD" << std::endl;
            auto shared_ptr1 = Make_Shared_Forward<A>(a);   // Должно произойти перемещение
            auto shared_ptr2 = Make_Shared_Forward<A>(A()); // Должно произойти перемещение
            std::cout << "--------------------" << std::endl;
        }
    }
    /*
     Variadic Template - шаблон с заранее неизвестным числом аргументов
     Оператор многоточия или элипсис  (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args).
     Например, std::tuple - может принять любое число входных параметров
     До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка)
     */
    {
        using namespace variadic_template;
        std::cout << "variadic template" << std::endl;
        
        MeasureProperty property = MeasureProperty::Two;
        CheckProperty(property, MeasureProperty::One, MeasureProperty::Two, MeasureProperty::Three);
        print("one", std::string{"two"}, 3, 4.0);
        [[maybe_unused]] auto vec = Vector(1, 2, 3, 4, 5);
        
        [[maybe_unused]] Sequence<IndexSequence<1, 2, 3>, int, char, double> sequence;
        /// Все пакеты в одном выражении распаковки должны иметь одинаковый размер
        // scale_and_print<1, 2>(3.14, 2, 3.0f); // Ошибка
        scale_and_print<1, 2, 3>(3.14, 2, 3.0f); // print(1 * 3.14, 2 * 2, 3 * 3.0)
        
        /*
         Примеси (mixIns) - это variadic CRTP, где шаблонный класс с заранее неизвестным числом аргументов наследуется от них.
         Применение: с помощью примесей + std::visit c использованием lambda-функций можно извлекать значения из vector<variant<...>>
         */
        {
            using namespace mixins;
            std::cout << "mixins" << std::endl;
            
            Mixin<std::vector<int>, std::string> mixin({ 1, 2, 3 }, "text");
            [[maybe_unused]] auto string_size = mixin.std::string::size();
            [[maybe_unused]] auto vector_size = mixin.std::vector<int>::size();
            
            Mixin{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                  [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                  [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
            }(1, 1, 1);
            
            CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic1(10);
            CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic2(20);
            CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic3(10);

            [[maybe_unused]] auto compare1 = variadic1 < variadic2;
            [[maybe_unused]] auto compare2 = variadic1 > variadic2;
            [[maybe_unused]] auto compare3 = variadic1 < variadic2;
            [[maybe_unused]] auto compare4 = variadic1 > variadic2;
            [[maybe_unused]] auto compare5 = variadic1 == variadic3;
            [[maybe_unused]] auto compare6 = variadic1 != variadic3;
        }
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
        std::cout << "fold expression" << std::endl;
        std::vector<int> numbers;
        
        [[maybe_unused]] auto sum_result1 = Sum(1, 2, 3);
        [[maybe_unused]] auto average_result = Average(1, 2, 3);
        [[maybe_unused]] auto norm_result = Norm(1, 2, 3);
        [[maybe_unused]] auto pow_sum_result = Pow_Sum(1, 2, 3);
        Push_To_Vector(numbers, 1, 2, 3, 4, 5);
        [[maybe_unused]] auto countArguments = CountArgs(1, "hello", 2.f);
        [[maybe_unused]] auto countTypes = CountTypes(1, "hello", 2.f);
        [[maybe_unused]] auto countArgumentsFunction = CountArgsFunction(Func).value;
        CheckTypes(int(1), std::string("hello"), double(2.0));
        Print_Strings("one", std::string{"two"});
    }
    // Lambda можно передавать шаблоны только при аргументах
    {
        using namespace LAMBDA;
        std::cout << "LAMBDA" << std::endl;
        
        int number = 10;
        // До C++20
        {
            auto Lambda = [&](auto&& number) // Сокращенный шаблон (auto)
            {
                MyFunction(std::forward<decltype(number)>(number));
            };
            
            Lambda(10);
            Lambda(number);
        }
        // C++20
        {
            auto Lambda = []<typename T>(T&& number)
            {
                 MyFunction(std::forward<T>(number));
            };
            
            Lambda(10);
            Lambda(number);
        }
        
        // Переменным число параметров (variadic template)
        auto Sum = []<typename... TArgs>(TArgs&&... args)
        {
            return (... + args);
        };
        
        [[maybe_unused]] auto sum = Sum(10, 11, 12);
        
        // Сокращенный шаблон (auto)
        auto Average = [&Sum](auto&&... args)
        {
            auto s =  Sum(args...);
            return s / sizeof...(args);
        };
        
        [[maybe_unused]] auto average = Average(10, 11, 12);
    }
    // callback
    {
        using namespace callback;
        std::cout << "callback" << std::endl;
        
        Example example;
        Permission(example, &Example::Method).Run();
        Permission(example, &Example::Method1).Run();
        Permission(example, &Example::Method2).Run();
    }
    /*
     Метафункция - это шаблонная (template) структура с constexpr членами, которые вычисляются во время компиляции.
     Стандартная библиотека (STL) придерживается соглашения, где в метафункциях указываются:
     Члены:
     - type - тип
     - value - возвращающееся значения, результат
     */
    {
        using namespace function;
        std::cout << "function" << std::endl;
        
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
    // invoke & apply
    {
        using namespace invoke_apply;
        
        int number1 = 1;
        int number2 = 2;
        Print example {10};
        std::tuple tuple {1, 1.5, 'c', "str"};
        
        /*
         std::invoke - шаблонная функция, оборачивающая и вызывающая ЛЮБОЙ объект с аргументами (ссылки, указатели, члены/функции-члены класса/структуры), в отличие от std::function и без SFINAE (constexpr).
         */
        {
            std::cout << "std::invoke" << std::endl;
            
            // 1 Способ: обычный
            {
                std::cout << "1 Способ: обычный" << std::endl;
                
                std::invoke(print<int, int>, 1, 2);
                std::invoke(print<int, int>, number1, number2);
                std::invoke(Print(), 1, 2);
                std::invoke(Print(), number1, number2);
                std::invoke(&Print::print<int, int>, example, 1, 2);
                std::invoke(&Print::print<int, int>, example, number1, number2);
                std::invoke(&Print::SetValue, &example, number2);
                [[maybe_unused]] auto value = std::invoke(&Print::GetValue, example);
                
                std::cout << std::endl;
            }
            // 2 Способ: lambda
            {
                std::cout << "2 Способ: lambda" << std::endl;
                
                std::invoke([&](){ print(1, 2); });
                std::invoke([&](){ print(number1, number2); });
                std::invoke([&](){ Print()(1, 2); });
                std::invoke([&](){ Print()(number1, number2); });
                std::invoke([&](){ example.print(1, 2); });
                std::invoke([&](){ example.print(number1, number2); });
                std::invoke([&](){ example.SetValue(number2); });
                [[maybe_unused]] auto value = std::invoke([&](){ return example.GetValue(); });
                
                std::cout << std::endl;
            }
            // 3 Способ: in template function
            {
                std::cout << "3 Способ: in function" << std::endl;
                
                CallInvoke(print<int, int>, 1, 2);
                CallInvoke(print<int, int>, number1, number2);
                CallInvoke(Print(), 1, 2);
                CallInvoke(Print(), number1, number2);
                CallInvoke(&Print::print<int, int>, example, 1, 2);
                CallInvoke(&Print::print<int, int>, example, number1, number2);
                CallInvoke(&Print::SetValue, &example, number2);
                [[maybe_unused]] auto value = CallInvoke(&Print::GetValue, example);
                
                std::cout << std::endl;
            }
        }
        /*
         std::apply - шаблонная функция, аналог std::invoke, но принимающая аргументы в качестве кортежа (std::tuple) и разворачивающая с помощью функции std::get<> Использование: когда нужно сохранить элементы в кортеж и использовать их позже (Например, в классе сохранять).
         */
        {
            std::cout << "std::apply" << std::endl;
            
            // 1 Способ: обычный
            {
                std::cout << "1 Способ: обычный" << std::endl;
                
                std::apply(print<int, int>, std::tuple{1, 2});
                std::apply(print<int, int>, std::tuple{number1, number2});
                std::apply(print<int, double, char, std::string>, tuple);
                std::apply(Print(), std::tuple{1, 2});
                std::apply(Print(), std::tuple{number1, number2});
                std::apply(Print(), tuple);
                std::apply(&Print::print<int, int>, std::tuple{example, 1, 2});
                std::apply(&Print::print<int, int>, std::tuple{example, number1, number2});
                std::apply(&Print::SetValue, std::tuple{example, number2});
                [[maybe_unused]] auto value = std::tuple(&Print::GetValue, std::tuple{example});
                
                std::cout << std::endl;
            }
            // 2 Способ: lambda
            {
                std::cout << "2 Способ: lambda" << std::endl;
                
                std::apply([](auto&& ...args)
               {
                    ((std::cout << args << ", "), ...);
                    std::cout << std::endl;
                }, std::tuple{1, 2});
                
                std::apply([&](auto&& ...args)
               {
                    print(std::forward<decltype(args)>(args)...);
                }, std::tuple{number1, number2});
                
                std::apply([&](auto&& ...args)
               {
                    print(std::forward<decltype(args)>(args)...);
                }, tuple);
                
                std::apply([&](auto&& ...args){ Print()(std::forward<decltype(args)>(args)...); }, std::tuple{1, 2});
                std::apply([&](auto&& ...args){ Print()(std::forward<decltype(args)>(args)...); }, std::tuple{number1, number2});
                std::apply([&](auto&& ...args){ Print()(std::forward<decltype(args)>(args)...); }, tuple);
                std::apply([&](auto&& ...args){ example.print(std::forward<decltype(args)>(args)...); }, std::tuple{1, 2});
                std::apply([&](auto&& ...args){ example.print(std::forward<decltype(args)>(args)...); }, std::tuple{number1, number2});
                std::apply([&](auto&& ...args){ example.SetValue(std::forward<decltype(args)>(args)...); }, std::tuple{number2});
                [[maybe_unused]] auto value = std::tuple([&](){ return example.GetValue(); });
                
                std::cout << std::endl;
            }
            // 3 Способ: in function
            {
                std::cout << "3 Способ: in function" << std::endl;
                
                CallApply(print<int, int>, std::tuple{1, 2});
                CallApply(print<int, int>, std::tuple{number1, number2});
                CallApply(print<int, double, char, std::string>, tuple);
                CallApply(Print(), std::tuple{1, 2});
                CallApply(Print(), std::tuple{number1, number2});
                CallApply(Print(), tuple);
                CallApply(&Print::print<int, int>, std::tuple{&example, 1, 2});
                CallApply(&Print::print<int, int>, std::tuple{&example, number1, number2});
                CallApply(&Print::SetValue, std::tuple{&example, number2});
                [[maybe_unused]] auto value = CallApply(&Print::GetValue, std::tuple{&example});
                
                std::cout << std::endl;
            }
        }
    }
    // metafunction
    {
        std::cout << "metafunction" << std::endl;
        
        [[maybe_unused]] auto is_same_1 = metafunction::is_same<int, int32_t>::value;
        [[maybe_unused]] auto is_same_2 = metafunction::is_same<int, std::string>::value;
        [[maybe_unused]] auto squareRes = metafunction::Square<5>::value;
        [[maybe_unused]] auto factorial = metafunction::Factorial<5>::value;
        [[maybe_unused]] auto fibonacci = metafunction::Fibonacci<7>::value;
        
        CONCEPT::Point point;
        std::vector<CONCEPT::Point> points = { { 2, 1 }, { 2, 2 }, { 1, 1 }, { 1, 2 } };
        
        std::array<CONCEPT::Point, 1> pointsArray = {points.front()};
        std::list<CONCEPT::Point> pointsList(points.begin(), points.end());
        
        std::cout << CONCEPT::metafunction::Info<decltype(points)>::type << std::endl;
        std::cout << CONCEPT::metafunction::Info<decltype(pointsArray)>::type << std::endl;
        std::cout << CONCEPT::metafunction::Info<decltype(pointsList)>::type << std::endl;
        std::cout << CONCEPT::metafunction::Info<decltype(point)>::type << std::endl;
    }
    /*
     CRTP (curiously recurring template pattern) - рекурсивный шаблон: класс Derived наследуется от шаблонного класса Base, использующего Derived как шаблонный параметр T. Через метод базового класса Base можно вызвать метод наследуемого класса Derived, используя статический полиморфизм вместо динамического полиморфизма (без таблицы виртуальных функций): static_cast<T*>(this)->method()
     Плюсы:
     - делается во время компиляции (compile), а не во время исполнения (runtime)
     - нет накладных расходов на создание таблиц виртуальных функций
     - время, затрачиваемое на выбор метода в таблице виртуальных функций
     Минусы:
     - у всех классов-наследников должны определены методы, которые вызываются в базовом классе
    */
    {
        std::cout << "CRTP" << std::endl;
        
        CRTP::Derived derived;
        derived.Interface1();
        derived.Interface2();
        
        [[maybe_unused]] auto& singleton1 = CRTP::SINGLETON::Singleton1::Instance();
        [[maybe_unused]] auto& singleton2 = CRTP::SINGLETON::Singleton2::Instance();
        
        CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic1(10);
        CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic2(20);
        CRTP::Variadic<CRTP::Counter, CRTP::Equal, CRTP::Compare> variadic3(10);

        [[maybe_unused]] auto compare1 = variadic1 < variadic2;
        [[maybe_unused]] auto compare2 = variadic1 > variadic2;
        [[maybe_unused]] auto compare3 = variadic1 < variadic2;
        [[maybe_unused]] auto compare4 = variadic1 > variadic2;
        [[maybe_unused]] auto compare5 = variadic1 == variadic3;
        [[maybe_unused]] auto compare6 = variadic1 != variadic3;
    }
    /*
     SFINAE (substitution failure is not an error) - при определении перегрузок функции ошибочные подстановки в шаблоны не вызывают ошибку компиляции, а отбрасываются из списка кандидатов на наиболее подходящую перегрузку.
     Правила:
     - SFINAE это обязательно связано с перегрузкой функций
     - это работает при автоматическом выводе типов шаблона (type deduction) по аргументам функции
     - некоторые перегрузки могут отбрасываться в том случае, когда их невозможно определить из-за возникающей синтаксической ошибки, компиляция при этом продолжается - без ошибок
     - отбросить могут только шаблон
     - SFINAE рассматривает только заголовок функции, ошибки в теле функции не будут пропущены
    */
    {
        // Square
        {
            using namespace SFINAE;
            std::cout << "SFINAE" << std::endl;
            
            int integer_num = 5;
            float floating_num = 5.0;
            bool boolean = true;
            Number<int> number_int(5);
            Number<float> number_float(5.0f);
            Number<float> number_boolean(true);
            
            {
                [[maybe_unused]] auto square1 = Square(integer_num); // Вызов int Square(int);
                [[maybe_unused]] auto square2 = Square(floating_num); // Вызов float Square(float);
                [[maybe_unused]] auto square3 = Square(boolean);  // Вызов bool Square(bool);
                //auto square4 = SFINAE::Square(number_int); // Ошибка: не найден operator*
            }

            // std::is_arithmetic
            {
                //auto square1 = IS_ARITHMETIC::Square(integer_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square2 = IS_ARITHMETIC::Square(floating_num); // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square3 = IS_ARITHMETIC::Square(boolean);  // Ошибка: выражение слева от ".value" должно представлять класс, структуру или объединение
                //auto square4 = IS_ARITHMETIC::Square(number_int); // Ошибка: бинарный "*": "const T" не определяет этот оператор или преобразование к типу приемлемо к встроенному оператору
            }

            // C++14: std::enable_if
            {
                [[maybe_unused]] auto square1 = ENABLE_IF::Square(integer_num); // Вызов int Square(int);
                [[maybe_unused]] auto square2 = ENABLE_IF::Square(floating_num); // Вызов float Square(float);
                [[maybe_unused]] auto square3 = ENABLE_IF::Square(boolean);  // Вызов bool Square(bool);
                [[maybe_unused]] auto square4 = ENABLE_IF::Square(number_int); // Вызов Number<int> Square(Number<int>);
                [[maybe_unused]] auto square5 = ENABLE_IF::Square(number_float); // Вызов Number<float> Square(Number<float>);
                [[maybe_unused]] auto square6 = ENABLE_IF::Square(number_boolean); // Вызов Number<bool> Square(Number<bool>);
            }

            // C++17: constexpr
            {
                [[maybe_unused]] auto square1 = CONSTEXPR::Square(integer_num); // Вызов int Square(int);
                [[maybe_unused]] auto square2 = CONSTEXPR::Square(floating_num); // Вызов float Square(float);
                [[maybe_unused]] auto square3 = CONSTEXPR::Square(boolean);  // Вызов bool Square(bool);
                [[maybe_unused]] auto square4 = CONSTEXPR::Square(number_int); // Вызов Number<int> Square(Number<int>);
                [[maybe_unused]] auto square5 = CONSTEXPR::Square(number_float); // Вызов Number<float> Square(Number<float>);
                [[maybe_unused]] auto square6 = CONSTEXPR::Square(number_boolean); // Вызов Number<bool> Square(Number<bool>);
                
                // TODO:
                CONSTEXPR::Convertible<1>();
            }
            // C++20: concept
            {
                [[maybe_unused]] auto square1 = SFINAE::CONCEPT::Square(integer_num); // Вызов int Square(int);
                [[maybe_unused]] auto square2 = SFINAE::CONCEPT::Square(floating_num); // Вызов float Square(float);
                [[maybe_unused]] auto square3 = SFINAE::CONCEPT::Square(boolean);  // Вызов bool Square(bool);
                [[maybe_unused]] auto square4 = SFINAE::CONCEPT::Square(number_int); // Вызов Number<int>
                [[maybe_unused]] auto square5 = SFINAE::CONCEPT::Square(number_float); // Вызов Number<float> Square(Number<float>);
                [[maybe_unused]] auto square6 = SFINAE::CONCEPT::Square(number_boolean); // Вызов Number<bool>Square(Number<bool>);
            }
        }
        
        // CONTAINER
        {
            using namespace SFINAE::CONTAINER;
            std::cout << "CONTAINER" << std::endl;
            
            constexpr const int number = 10;
            constexpr std::string_view value = "10";
            std::vector<int> numbers { 1,2,3,4,5 };
            [[maybe_unused]] auto has_const_iterator1 = has_const_iterator<std::vector<int>>::value;
            [[maybe_unused]] auto has_const_iterator2 = has_const_iterator<int>::value;
            
            [[maybe_unused]] auto has_begin_value1 = has_begin_end<std::vector<int>>::begin_value;
            [[maybe_unused]] auto has_end_value1 = has_begin_end<std::vector<int>>::end_value;
            [[maybe_unused]] auto has_begin_value2 = has_begin_end<int>::end_value;
            [[maybe_unused]] auto has_end_value2 = has_begin_end<int>::begin_value;
            
            // std::enable_if
            {
                SFINAE::CONTAINER::ENABLE_IF::Print(numbers);
                SFINAE::CONTAINER::ENABLE_IF::Print(number);
                SFINAE::CONTAINER::ENABLE_IF::Print(value);
            }
            // constexpr
            {
                SFINAE::CONTAINER::CONSTEXPR::Print(numbers);
                SFINAE::CONTAINER::CONSTEXPR::Print(number);
                SFINAE::CONTAINER::CONSTEXPR::Print(value);
            }
            
            /*
             Концепт (concept) - это имя для ограничения, которое используется вместо слов class или typename в конструкции с template.
             Ограничение — это шаблонное булево выражение. Концепты улучшают читаемость кода и облегчает поиск ошибок. Концепции можно использовать для перегрузки функций, специализации шаблонов и создания метафункций. Концепты компилируются быстрее обычного SFINAE (std::enable_if и constexpr) и условия в них можно расширять.
             Версия C++20 вернулась обратно к двум функциям, но теперь код намного читабельнее, чем с std::enable_if.
             requires - пользовательское ограничение.
            */
            {
                using namespace CONCEPT;
                std::cout << "CONCEPT" << std::endl;

                Point point;
                DerivedPoint derivedPoint;
                NoDerivedPoint noderivedPoint;
                std::vector<Point> points = { { 2, 1 }, { 2, 2 }, { 1, 1 }, { 1, 2 } };
                
                // common
                {
                    common::Print(points);
                    common::Print(0u);
                    common::Print(0);
                    common::Print(0.f);
                    common::Print(1u);
                    common::Print(1.f);
                    common::Print(-1);
                    common::Print(-1.f);
                    common::Print(1.1);
                    common::Print(-1.1);
                    
                    common::ConvertTo(1, 1); // int to int
                    common::ConvertTo(1.f, 1); // float to int
                    common::ConvertTo(double(1.0), 1); // double to int
                    common::ConvertTo('1', 1); // char to int
                    // common::ConvertTo(std::string("str"), 1); // Нельзя сконвертировать int в std::string
                    
                    common::Is_Base_Point(derivedPoint); // std::is_base_of(Point, DerivedPoint) == true
                    // common::Is_Base_Point(noderivedPoint); // std::is_base_of(Point, DerivedPoint) == false
                    
                    int number1 = 1;
                    double number2 = 1.0;
                    std::string str = "str";
                    
                    auto pointer1 = common::variadic::constructArgs<NoDerivedPoint>(); // nullptr почему-то для конструктора default
                    auto pointer2 = common::variadic::constructArgs<NoDerivedPoint>(number1, number2, str);
                    auto pointer3 = common::variadic::constructArgs<NoDerivedPoint>(1, 1.0, "str");
                    auto pointer4 = common::variadic::constructArgs<NoDerivedPoint>(1); // nullptr
                    auto pointer5 = common::variadic::constructArgs<NoDerivedPoint>(1.0); // nullptr
                    auto pointer6 = common::variadic::constructArgs<NoDerivedPoint>("str"); // nullptr
                    
                    common::variadic::Print_Numeric((int)1, (double)2.0, (float)3.0);
                    // auto abs3 = common::abs((float)1.0); // Ошибка: не указан тип float
                    
                    [[maybe_unused]] auto allArithmetic1 = common::variadic::Has_All_Arithmetic(5, true, 5.5, false); // false
                    [[maybe_unused]] auto allArithmetic2 = common::variadic::Has_All_Arithmetic(5, 5.5); // true
                    [[maybe_unused]] auto anyArithmetic1 = common::variadic::Has_Any_Arithmetic(5, true, 5.5, false); // true
                    [[maybe_unused]] auto anyArithmetic2 = common::variadic::Has_Any_Arithmetic(true, false); // true
                    [[maybe_unused]] auto noneArithmetic1 = common::variadic::Has_None_Arithmetic(5, true, 5.5, false); // false
                    [[maybe_unused]] auto noneArithmetic2 = common::variadic::Has_None_Arithmetic(5, 5.5); // false
                    [[maybe_unused]] auto noneArithmetic3 = common::variadic::Has_None_Arithmetic(true, false); // false
                    
                    [[maybe_unused]] auto foundPoint = common::lambda::FindSubPoint(points, Point{ 0, 0 }, Point{ 2, 1 }); // Point{ 2, 1 }
                }
                // custom
                {
                    [[maybe_unused]] auto operation1 = custom::details::Operation<int>;  // true
                    [[maybe_unused]] auto operation2 = custom::details::Operation<char>; // true
                    [[maybe_unused]] auto operation3 = custom::details::Operation<std::string>; // false
                    [[maybe_unused]] auto operation4 = custom::details::Operation<Point>; // false
                    
                    custom::Sort(points.begin(), points.end());
                    custom::Print(points);
                    custom::Print(1.1);
                }
                /*
                 Сокращенный шаблон (auto или Concept auto) - шаблонная функция, которая содержит auto в качестве типа аргумента или возвращающегося значения
                 Плюсы:
                 - упрощает синтаксис
                 - не нужно писать template
                 Минусы
                 - если забыть сделать return в функции, то функция станет void
                 - можно вернуть не тот тип
                 Решение: использовать концепты для точного возвращения нужного типа
                 */
                {
                    // auto resultReturn = CONCEPT::AUTO::NothingReturn(); // Ошибка NothingReturn ничего не возвращает
                    CONCEPT::AUTO::DrawShape(point);
                    [[maybe_unused]] auto shape1 = CONCEPT::AUTO::GetShape1();
                    [[maybe_unused]] auto shape2 = CONCEPT::AUTO::GetShape2();
                    
                    [[maybe_unused]] auto abs1 = CONCEPT::AUTO::ABS((int)1);
                    [[maybe_unused]] auto abs2 = CONCEPT::AUTO::ABS((double)1.0);
                    
                    [[maybe_unused]] auto size1 = CONCEPT::AUTO::GetSize(points);
                    [[maybe_unused]] auto size2 = CONCEPT::AUTO::GetSize(1.0);
                    
                    CONCEPT::AUTO::Print(1.1);
                }
                // metafunction
                {
                    std::array<Point, 1> pointsArray = {points.front()};
                    std::list<Point> pointsList(points.begin(), points.end());
                    
                    std::cout << CONCEPT::metafunction::Info<decltype(points)>::type << std::endl;
                    std::cout << CONCEPT::metafunction::Info<decltype(pointsArray)>::type << std::endl;
                    std::cout << CONCEPT::metafunction::Info<decltype(pointsList)>::type << std::endl;
                    std::cout << CONCEPT::metafunction::Info<decltype(point)>::type << std::endl;
                }
            }
        }
    }
    /*
     matching - паттерн сопоставляющий типы, который можно рассматривать как обобщение оператора switch-case
     */
    {
        std::cout << "matching" << std::endl;
        using namespace matching;
        
        /// C++11
        {
            std::cout << "C++11" << std::endl;
            using namespace C11;
        
            Match([](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                  [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                  [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                  [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                  )(1, 1, 1);
        }
        /*
         C++17: Примеси (mixIns) - это variadic CRTP, где шаблонный класс с заранее неизвестным числом аргументов наследуется от них.
         Применение: с помощью примесей + std::visit c использованием lambda-функций можно извлекать значения из vector<variant<...>>
         */
        {
            std::cout << "C++17" << std::endl;
            using namespace C17;
        
            /// 1 Вариант
            {
                std::cout << "first implementation" << std::endl;
                using namespace first_implementation;
                
                Match([](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                      [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                      [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                      [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                      [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                      )(1, 1, 1);
            }
            /// 2 Вариант
            {
                std::cout << "second implementation" << std::endl;
                using namespace second_implementation;
                
                Match{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                      [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                      [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                      [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                      [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                }(1, 1, 1);
            }
            /// 3 Вариант
            {
                std::cout << "third implementation" << std::endl;
                using namespace third_implementation;
                
                /// 1 Способ: обычный
                {
                    Match{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                          [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                          [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                          [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                          [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                    }(1, 1, 1);
                }
                
                /// 2 Способ: std::variant + std::visit
                {
                    std::vector<std::variant<int, double, std::string>> vec = {10, 10.0, "str"};
                    for (const auto& v : vec)
                    {
                        std::visit(Match{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                                         [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                                         [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                         [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                         [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                        }, v);
                    }
                }
                /// 3 Способ: несколько std::variant + std::visit
                {
                    std::variant<int, double, std::string> var1, var2, var3;
                    var1 = 10;
                    var2 = 10.0;
                    var3 = "str";
                    std::visit(Match{[](int number1, int number2) { std::cout << "int: " << number1 << " " << number2 << std::endl; },
                                     [](double number1, double number2) { std::cout << "double: " << number1 << " " << number2 << std::endl; },
                                     [](int number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                     [](double number1, auto number2)  { std::cout << "float: " << number1 << " " << number2 << std::endl; },
                                     [&](auto... values) { std::cout << "other types: "; ((std::cout << values << " "), ...) << std::endl; }
                    }, var1, var2, var3);
                }
            }
        }
    }
    /*
     Tuple (Кортеж) - коллекция элементов с фиксированным размером, содержащая разнородные значения. Для реализации кортежа используется идиома Head/Tail с помощью Variadic Template - шаблон с заранее неизвестным числом аргументов.
     */
    {
        using namespace tuple;
        std::cout << "tuple" << std::endl;
        
        /// first implementation
        {
            using namespace first_implementation;
            std::cout << "first implementation" << std::endl;
            
            static_assert(Tuple<>::value == 0, "must be 0");
            static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
            
            Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
            Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
            Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
            
            [[maybe_unused]] auto value_int = Get<0>(tuple);
            [[maybe_unused]] auto value_double = Get<1>(tuple);
            [[maybe_unused]] auto value_char = Get<2>(tuple);
            [[maybe_unused]] auto value_string = Get<3>(tuple);
            [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
            [[maybe_unused]] auto size = tuple.Size();
            [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
            [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
        }
        /// second implementation
        {
            using namespace second_implementation;
            std::cout << "second implementation" << std::endl;
            
            static_assert(Tuple<>::value == 0, "must be 0");
            static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
            
            Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
            Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
            Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
            
            [[maybe_unused]] auto value_int = Get<0>(tuple);
            [[maybe_unused]] auto value_double = Get<1>(tuple);
            [[maybe_unused]] auto value_char = Get<2>(tuple);
            [[maybe_unused]] auto value_string = Get<3>(tuple);
            [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
            [[maybe_unused]] auto size = tuple.Size();
            [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
            [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
        }
        /// third implementation
        {
            using namespace third_implementation;
            std::cout << "third implementation" << std::endl;
            
            static_assert(Tuple<>::value == 0, "must be 0");
            static_assert(Tuple<int, double, std::string&>::value == 3, "must be 3");
            
            Tuple<int, double, char, std::string> tuple(1, 10.0, 'c', "abc");
            Tuple tuple_deduction1{1, 10.0, 'c', "ABC"};
            Tuple tuple_deduction2 = Tuple(1, 10.0, 'c', "ABC");
            
            [[maybe_unused]] auto value_int = Get<0>(tuple);
            [[maybe_unused]] auto value_double = Get<1>(tuple);
            [[maybe_unused]] auto value_char = Get<2>(tuple);
            [[maybe_unused]] auto value_string = Get<3>(tuple);
            [[maybe_unused]] auto make_tuple = Make_Tuple(1, 10.0, 'c');
            [[maybe_unused]] auto size = tuple.Size();
            [[maybe_unused]] auto ebo1 = sizeof(Tuple<dummy<0>, dummy<1>, dummy<2>>); // 1
            [[maybe_unused]] auto ebo2 = sizeof(Tuple<dummy<0>, dummy<0>, dummy<0>>); // 3
            
            [[maybe_unused]] auto make_ref_tuple = Make_Ref_Tuple(value_int, value_double, value_char, value_string);
            Get<0>(make_ref_tuple) = 10;
            Get<1>(make_ref_tuple) = 100.0;
            Get<2>(make_ref_tuple) = 'C';
            Get<3>(make_ref_tuple) = "ABC";
        }
    }
    
    return 0;
}
