# Template
Шаблон — паттерн который создает классы или функции различных типов. Шаблон - это лишь макет, по которому генерируется код во время компиляции. При виде такой конструкции: ```my_swap<T>``` компилятор сам создаст функцию my_swap с необходимым типом. Это называется инстанцирование шаблона. То есть при виде ```my_swap<int>``` компилятор создаст функцию my_swap в которой T поменяет на int, а при виде ```my_swap<double>``` будет создана функция с типом double. Если где-то дальше компилятор опять встретит ```my_swap<int>```, то он ничего генерировать не будет, т.к. код данной функции уже есть (шаблон с данным параметром уже инстанцирован).
Таким образом, если мы инстанцируем этот шаблон 2 раза с разными типами, то компилятор создаст 2 разные функции. <br/>
Шаблон — помогает сэкономить время выполнения (rutime) засчет времени компиляции (compile), например, constexpr, и использовать минимальное кол-во кода.

## 4 типа шаблонов
- шаблоны функций: ``` template<typename T> T max(T &a, T &b) { return a > b ? a : b; } ```
- шаблоны классов: ``` template<class T> class { T t; }; ```
- шаблоны переменных: ``` template<typename T> constexpr T pi = T{3.141592653589793238462643383L}; ```
- Non-type параметры: ``` template<class T, size_t N> class array; ```. Non-type template parameter (шаблонные аргументы не являющиеся типами) или (шаблонные аргументы-константы) - в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции. C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения. C++17: можно писать auto для non-type template параметров.

Шаблоны-члены не могут быть virtual в классах.

Плюсы:
- обобщение разных типов данных, таких конейнеров std::vector, std::map
- сокращение повторения кода
- cтатический полиморфизм - перегрузка функций (SFINAE)
- более безопасны в использовании (проверка идет во время компиляции)
- в функции/классах можно передавать неограниченное число аргументов (variadic template): template<typename… Args>

Минусы: 
- затрудняют отладку кода
- неясные ошибки во время компиляции
- увеличение время компиляции размера скомпилированного двоичного файла, если определять шаблон в .h файле.
 
## Инстанцирование шаблона
Это генерация кода функции или класса через подстановку параметров в шаблон. При инстанцировании 3 разных типов: int, double, string, компилятор создает три разные функции/классы.

2 Вида инстанцирования:

### 1. Implicit Instantiation
Неявное инстанцирование - происходит при вызове функции или создании объекта класса (обычно в .h файлах)

Плюсы:
- не нужно явно инстанцировать все типы в .cpp файле
- не нужно явно инстанцировать все типы, если шаблон инициализирован и определен в одном .cpp файле в том месте, где он вызывается
Минусы:
- Размещение шаблонов в .h файлах может привести к загромождению размера кода и размера скомпилированного двоичного файла.

### 2. Explicit instantiation
Явное инстанцирование - с помощью резервированного слова template (обычно в .cpp файлах). Каждая единица трансляции транслируется отдельно, а потом всё линкуется. Инстанцирование шаблонов (подстановка) происходит до линковки. Компилятор при генерировании кода должен видеть определение шаблона (а не только объявление), так и определения типов, которые подставляются в шаблон. Поэтому если класс/функция инициализированы в .h файле, а реализация в .cpp файле, то при одновременной комплияции двух .cpp файлов компилятор не запомнит подставляемый тип в одном .cpp файла, где вызывается шаблон, во время компиляции другого .cpp файла, где находится реализация метода/класса. В .cpp файле, где реализация метода/класса нужно сделать явное инстанцирование. 

 Плюсы:
 - cокрытие явной реализации класса/метода в .cpp файле
 - предотвращение переопределение объекта (экономит время и размер)
 
 Минусы:
 - Если не создать явное инстанцирование всех типов в .cpp файле, то будет ошибка: undefined reference
 - Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки

#### Дополнение
Явно инстанцированная шаблонная функция/класс могут иметь копии .cpp файлах, что может тормозить сам процесс компиляции и сборки. Можно указать явное объявление инстанциации с помощью extern в .h файле и одно определение в .cpp файле, и тогда компилятору не нужно будет инстанцировать шаблон в каждой единице трансляции, что уменьшает время компиляции.
 
#### Компилятор в приоритете выбирает более специлизированный шаблон <br>
Специализации шаблонов по приоритетам:
1. Полная специализация (explicit (или full) specialization) - напоминает наследование или перегрузку - template<> class vector<bool>, тогда компилятор не будет дописывать класс под конкретный тип данных, а вызовет уже объявленную специализацию, таким образом мы может запретить для какого-то типа определенные действия или дополнить, в отличие от остальных типов
2. Частичная специализация (partial specialization) - только для классов template<Class A> class vector<bool, A>.
Для методов нельзя, но это можно обойти сделать в частичном специализированном классе static метод.
3. Основной шаблон (primary template) - vector<T>
 
## Variadic Template 
Шаблон с заранее неизвестным числом аргументов.
Оператор многоточия или элипсис (…), когда он стоит слева от имени параметра функции, он объявляет набор параметров (template<typename... Args>), когда стоит справа от шаблона или аргумента вызова функции, он распаковывает параметры в отдельные аргументы (Args&... args). Например, std::tuple - может принять любое число входных параметров.

До C++17: для развёртывания пачки параметров требуется рекурсивное инстанциирование шаблонов с помощью stub-функции (заглушка).
 
## Fold expression
Выражение свертки - шаблон с заранее неизвестным числом аргументов (variadic template). Свертка – это функция, которая применяет заданную комбинирующую функцию к последовательным парам элементов в списке и возвращает результат. Любое выражение свёртки должно быть заключено в скобки и в общем виде выглядит так: (выражение содержащее пачку аргументов). Выражение внутри скобок должно содержать в себе нераскрытую пачку параметров и один из следующих операторов:
```
+  -  *  /  %  ^  &  |  =  <  >  <<  >>
+=  -=  *=  /=  %=  ^=  &=  |=  <<=  >>=
==  !=  <=  >=  &&  ||  ,  .*  ->*
```
 
## Template argument deduction
Способность шаблоннов определять тип передаваемых аргументов без явного указания типа: вместо foo<...>(...) можно foo(...).

До C++17 при инстанцировании шаблона функции нужно было явно указывать типы аргументы: foo<...>(...).

## auto или Concept auto 
Сокращенный шаблон - шаблонная функция, которая содержит auto в качестве типа аргумента или возвращающегося значения.

Плюсы:
- упрощает синтаксис
- не нужно писать template

Минусы:
- если забыть сделать return в функции, то функция станет void
- можно вернуть не тот тип <br>
Решение: использовать концепты для точного возвращения нужного типа

## Non-type template parameter
Шаблонные аргументы не являющиеся типами - шаблонные аргументы-константы, в качестве аргументов шаблонов могут выступать константы времени на этапе комплияции. <br>
C++14: способ передать non-type template параметр с неизвестным типом была передача двух параметров – типа и значения. <br>
C++17: можно писать auto для non-type template параметров. <br>
С++20: можно работать с вещественными числами. <br>

## forward
std::forward - идеальной передача (perfect forwarding).

### Свойства:
- std::forward РАБОТАЕТ с T&& с НЕконстантными ссылками
- std::forward НЕ работает с const T& - для всех типов ссылок (rvalue, const rvalue, lvalue, const lvalue), т.к. rvalue(T&&) -> lvalue (T&) на основе сжатии ссылок (T&(A&&) = A& // && отбрасывается).
- std::forward НЕ будет работает c const&& для lvalue

std::forward используется ТОЛЬКО в шаблонах:
```
template<typename T>
T&& forward(T&& param)
{
    if (is_lvalue_reference<T>::value)
        return param;
    else
        return move(param);
}
```

### Тип typename T имеет 2 перегрузки:
1. lvalue-ссылка (T = const A& или T = A&)
2. rvalue-ссылка (T = A&&) <br/>
Дополнительный: <br/>
3. Бессылочный тип (T = A*) - указатель

Идеальная передача позволяет создавать функции-обертки, где lvalue-выражения копируются (просто возвращается lvalue), а rvalue-выражения перемещаются (std::move). <br/>
Использование:
```
class Example
{
   std::string field;
   template <class T>
   Example(T&& x) : field(std::forward<T>(x)) {}
};
```

## Reference collapse
Сжатие ссылок, которое используется только в шаблонах и определяет поведение (отбрасывает не нужные &) при появлении ссылки на ссылку. Меньшее число (&) - выигрывает у большего числа (&&). <br/>
Пример:<br/>
T инстанцирован A&&, то T&& (A&& &&) = A&& // && отбрасывается

### Правила:
1. T&(A&)  = A& // & отбрасывается
2. T&&(A&) = A& // && отбрасывается
3. T&(A&&) = A& // && отбрасывается
4. T&&(A&&) = A&& // && отбрасывается, но тип xvalue->lvalue

Универсальной ссылка - аргумент типа T&&, где T — шаблонный тип, может принимать оба типа ссылок (A& и A&&). <br/>
Пример:
```
template<typename T> 
function(T&& arg) // arg - lvalue всегда, т.к. T&& arg - xvalue, поэтому будет вызываться конструктор копирования
```

### Отличие std::forward от std::move: 
std::move - приводит lvalue к rvalue, std::forward - lvalue просто возвращает lvalue, а rvalue – возвращает std::move(rvalue).

## Metafunction
Метафункция - это шаблонная (template) структура с constexpr членами, которые вычисляются во время компиляции.
Стандартная библиотека (STL) придерживается соглашения, где в метафункциях указываются: <br/>
Члены: <br/>
- type - тип
- value - возвращающееся значения, результат

## Concept
Концепт (concept) - это имя для ограничения, которое используется вместо слов class или typename в конструкции с template. <br/>
Ограничение — это шаблонное булево выражение. Концепты улучшают читаемость кода и облегчает поиск ошибок. Концепции можно использовать для перегрузки функций, специализации шаблонов и создания метафункций. Концепты компилируются быстрее обычного SFINAE (std::enable_if и constexpr) и условия в них можно расширять. <br/>
Версия C++20 вернулась обратно к двум функциям, но теперь код намного читабельнее, чем с std::enable_if. <br/>
requires - пользовательское ограничение.

## using vs typedef
typedef не позволяет сделать alias (псевдоним) для template. Лучше полностью отказаться от typedef и перейти на using

# Разница шаблонов (templates) от макросов (macros)
## Макрос
Макрос - это механизм преобразования кода препроцессором в текст, выполняемые ДО передачи этого тескта компилятору. С текстом программы работает препроцессор, для которого программа - набор символов (букв, цифр, знаков для операторов, пробелов, и т.д.). <br/>
#include - указание "вставить вместо макроса весь текст содержащийся в файле" <br/>
#define - указание "встречая идентификатор определяющий макрос, вставить текст следующий за макросом с заменой аргументов передаваемым по месту использования текстом".

## Шаблоны
Шаблон - компилятор полностью разбирает шаблон, выполняет проверку корректности на уровне типов, переменных, подстановок шаблонов, для функций выполняет проверку правил перегрузки, и т.д.

## Отличие 
Шаблоны предпочтительнее макросов.
- шаблоны обрабатываются копилятором, а макросы обрабатываются препроцессором
- для макросов недоступны специализации и перегрузки
- макросы выполняют текстовую подстановку аргументов, шаблоны лексически и синтаксически проверяются компилятором
- для макросов не предусмотрено ничего похожего на отложенное (неявное) инстанцирование, которое очень востребовано при использовании шаблонов
- макросы существенно проблематичнее в отладке и поиске ошибок, чем шаблоны
- компиляция шаблонов происходит быстрее, чем в эквивалентной версии макроса

# Patterns
Шаблоны — помогают сэкономить время выполнения (rutime) засчет времени компиляции (compile), например, constexpr, и использовать минимальное кол-во кода.

## CRTP
Сuriously Recurring Template Pattern - рекурсивный шаблон: класс Derived наследуется от шаблонного класса Base, использующего Derived как шаблонный параметр T. Через метод базового класса Base можно вызвать метод наследуемого класса Derived, используя статический полиморфизм вместо динамического полиморфизма (без таблицы виртуальных функций): <br>
```static_cast<T*>(this)->method()```

Плюсы:
- делается во время компиляции (compile), а не во время исполнения (runtime)
- нет накладных расходов на создание таблиц виртуальных функций
- время, затрачиваемое на выбор метода в таблице виртуальных функций

Минусы:
- у всех классов-наследников должны определены методы, которые вызываются в базовом классе

## SFINAE
Substitution Failure Is Not An Error - при определении перегрузок функции ошибочные подстановки в шаблоны не вызывают ошибку компиляции, а отбрасываются из списка кандидатов на наиболее подходящую перегрузку.

Правила:
- SFINAE это обязательно связано с перегрузкой функций
- это работает при автоматическом выводе типов шаблона (type deduction) по аргументам функции
- некоторые перегрузки могут отбрасываться в том случае, когда их невозможно определить из-за возникающей синтаксической ошибки, компиляция при этом продолжается - без ошибок
- отбросить могут только шаблон
- SFINAE рассматривает только заголовок функции, ошибки в теле функции не будут пропущены

## Matching
Паттерн сопоставляющий типы, который можно рассматривать как обобщение оператора switch-case.

# Сайты: 
[Языковые новшества C++17. Часть 1. Свёртка и выведение](http://scrutator.me/post/2017/08/11/cpp17_lang_features_p1.aspx) <br/>
[А есть ли функция?](http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx) <br/>
[Шаблоны](https://cpp-kt.github.io/cpp-notes/14_templates.html) <br/>
[What are C++ variadic templates and fold expressions?](https://iamsorush.com/posts/cpp-variadic-template/) <br/>
[Why can templates only be implemented in the header file?](https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file/495056#495056) <br/>
[C++20 idioms for parameter packs](https://www.scs.stanford.edu/~dm/blog/param-pack.html) <br/>
[Explicit template instantiation - when is it used?](https://stackoverflow.com/questions/2351148/explicit-template-instantiation-when-is-it-used) <br/>
[Парадигмы ООП. Полиморфизм (статический, динамический). Инкапсуляция. Наследование. Примеры динамического и статического полиморфизмов](https://gist.github.com/sergeysablin99/1eb61b51fb58d5ae0a58ac1424c249c6) <br/>
[extern template](https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl) <br/>
[metafunction](https://habr.com/ru/articles/337590/) <br/>
[metafunction](http://scrutator.me/post/2017/04/10/has_function_metaprogramming.aspx) <br/>
[matching](https://habr.com/ru/articles/282630/) <br/>
