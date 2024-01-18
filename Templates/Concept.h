#ifndef Concept_h
#define Concept_h

#include <algorithm>

/*
 Сайты:
 Концепты: https://habr.com/ru/companies/yandex_praktikum/articles/556816/ 
           https://iamsorush.com/posts/concepts-cpp/
 */

 /*
  Концепт (concept) - это имя для ограничения, которое используется вместо слов class или typename в конструкции с template.
  Ограничение — это шаблонное булево выражение. Концепты улучшают читаемость кода и облегчает поиск ошибок. Концепции можно использовать для перегрузки функций, специализации шаблонов и создания метафункций. Концепты компилируются быстрее обычного SFINAE (std::enable_if и constexpr) и условия в них можно расширять.
  Версия C++20 вернулась обратно к двум функциям, но теперь код намного читабельнее, чем с std::enable_if.
  requires - пользовательское ограничение
 */

namespace CONCEPT
{
    struct Point
    {
        int x = 0;
        int y = 0;

        auto operator<=>(const Point&) const = default; // Условие: без этого concept не будет работать


        auto Сoefficient(auto сoefficient)
        {
            return (x + y) * сoefficient;
        }

        void Draw() const
        {

        }
    };

    struct DerivedPoint : public Point
    {

    };

    struct NoDerivedPoint
    {
        NoDerivedPoint() {}
        NoDerivedPoint(int number1, double number2, std::string str) :
            _number1(number1),
            _number2(number2),
            _str(str)
        {}

    private:
        int _number1 = 0;
        double _number2 = 0.0;
        std::string _str;
    };

    namespace common
    {
        namespace concepts
        {
            template <class T>
            concept Integral = std::is_integral<T>::value; // Условие: является ли тип целочисленным

            template <class T>
            concept Float = std::is_floating_point<T>::value; // Условие: является ли тип с плавающей запятой

            template <class T>
            concept SignedIntegral = Integral<T> && std::is_signed<T>::value; // Условие: является ли тип отрицательным целым

            template <class T>
            concept UnsignedIntegral = Integral<T> && std::is_unsigned<T>::value; // Условие: является ли тип положительным целым

            template <class T>
            concept SignedFloat = Float<T> && std::is_signed<T>::value; // Условие: является ли тип отрицательным с плавающей запятой

            template <class T>
            concept UnsignedFloat = Float<T> && std::is_unsigned<T>::value; // Условие: является ли тип положительным с плавающей запятой

            template <class T, class U>
            concept Convertible_To = std::convertible_to<T, U>; // Условие: можно ли сконвертировать тип в другой тип

            template<class T, class U>
            concept Derived = std::is_base_of<U, T>::value; // Условие: является ли производный класс к базовому классу

            template<typename T>
            concept Arithmetic = std::is_arithmetic<T>::value; // Условие: является ли тип арифметическим

            template <typename T>
            concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>; // Условие: является ли тип с плавающей запятой и целочисленным

            template<typename... TArgs>
            concept Numerics = (Numeric<TArgs> && ...);
        }

        /// 1 Способ: без использования requires, использовать для custom условий
        template<concepts::SignedIntegral T>
        void Print(const T& value)
        {
            std::cout << "Целочисленное Signed число: " << value << std::endl;
        }

        /// 2 Способ: использование сокращенного шаблона auto. До C++20 они были доступны только в лямбдах. Теперь можно использовать auto
        void Print(const concepts::UnsignedIntegral auto& value)
        {
            std::cout << "Целочисленное Unsigned число: " << value << std::endl;
        }

        /// 3 Способ: НЕобязательное использование requires ПЕРЕД функцией
        template<class T>
        requires concepts::SignedFloat<T>
        void Print(const T& value)
        {
            std::cout << "Дробное Signed число: " << value << std::endl;
        }

        /// 4 Способ: НЕобязательное использование requires ПОСЛЕ функции
        template<class T>
        void Print(const T& value) requires concepts::UnsignedFloat<T>
        {
            std::cout << "Дробное Unsigned число: " << value << std::endl;
        }

        /// 5 Способ: ОБЯЗАТЕЛЬНОЕ использование requires ПЕРЕД функцией
        template<class T, class U>
        requires concepts::Convertible_To<T, U>
        constexpr void ConvertTo(const T& lhs, const U& rhs)
        {
            std::cout << "Можно сконвертировать тип : " << typeid(T).name() << " в тип: " << typeid(U).name() << std::endl;
        }

        /// 6 Способ: Сокращенный шаблон auto, использовать В ПРИОРИТЕТЕ!
        void Print(const std::ranges::common_range auto& container)
        {
            if constexpr (requires {std::is_convertible_v<decltype(container.front()), Point>; }) // requires можно использовать прямо в теле функции или метода
            {
                for (const auto& elem : container)
                {
                    std::cout << "x: " << elem.x << ", y: " << elem.y << std::endl;
                }
            }
            else
            {
                for (const auto& elem : container)
                {
                    std::cout << elem << std::endl;
                }
            }
        }

        /*
         В С++20 пока не работает для контейнеров
         constexpr double Print(const std::vector<Numeric auto>& vec)
         {
             const double sum = std::accumulate(vec.begin(), vec.end(), 0.0);
             return sum / static_cast<double>(vec.size());
         }
         */

         /*
          Концепт может иметь несколько шаблонных параметров. При использовании концепта нужно задать все, кроме одного — того, которое проверяется на ограничение.
          */
        template<concepts::Derived<Point> Class>
        void Is_Base_Point(const Class& object)
        {

        }

        namespace variadic
        {
            template <class T, typename... TArgs>
                requires std::is_constructible_v<T, TArgs...> // Условие: проверка списка аргументов для создания типа T
            std::unique_ptr<T> constructArgs(TArgs&&... args)
            {
                return std::make_unique<T>(std::forward<TArgs>(args)...);
            }

            // Заглушка для constructArgs в случае неудачи
            template <class T, typename... TArgs>
            std::unique_ptr<T> constructArgs(...)
            {
                return nullptr;
            }

            template<typename... Args>
            void Print_Numeric(Args&&... args) requires concepts::Numerics<Args...>
            {
                std::cout << "Числа: ";
                ((std::cout << args << " "), ...);
                std::cout << std::endl;
            }

            template<concepts::Arithmetic... TArgs>
            bool Has_All_Arithmetic(TArgs&&... args)
            {
                return (... && args);
            }

            template<concepts::Arithmetic... TArgs>
            bool Has_Any_Arithmetic(TArgs&&... args)
            {
                return (... || args);
            }

            template<concepts::Arithmetic... TArgs>
            bool Has_None_Arithmetic(TArgs&&... args)
            {
                return !(... || args);
            }
        }
    }

    namespace custom
    {
        namespace concepts
        {
            template<class T>
            concept Iterator = requires(const T & lhs, const T & rhs) // Условие: проверка на возможность сравнения
            {
                {lhs < rhs} -> std::convertible_to<bool>;
            };

            template<class T>
            concept HasBeginEnd = requires(const T & container) // Условие: проверка на контейнер
            {
                container.begin();
                container.end();
            };

            template<typename T>
            concept Operation = requires (const T & item)
            {
                item + item; item - item; item* item; // Условие: сложение, вычитание, умножение
            };
        }

        template<concepts::Iterator It>
        void Sort(const It& begin, const It& end)
        {
            std::sort(begin, end);
        }

        template<concepts::HasBeginEnd T>
        void Print(const T& container)
        {
            if constexpr (requires {std::is_convertible_v<decltype(container.front()), Point>; }) // requires можно использовать прямо в теле функции или метода
            {
                for (const auto& elem : container)
                {
                    std::cout << "x: " << elem.x << ", y: " << elem.y << std::endl;
                }
            }
            else
            {
                for (const auto& elem : container)
                {
                    std::cout << elem << std::endl;
                }
            }
        }

        void Print(const auto& value)
        {
            std::cout << "Число: " << value << std::endl;
        }
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
    namespace AUTO
    {
        namespace concepts
        {

            template<class T>
            concept HasBeginEnd = requires(const T & container) // Условие: проверка на контейнер
            {
                container.begin();
                container.end();
            };

            template<typename T>
            concept Shape = requires(const T & point)
            {
                { point.Draw() } -> std::same_as<void>;
            };

            template<typename T>
            concept hasSize = requires (const T & container) { container.size(); }; // Условие: имеет ли тип метод size()

            template <typename T, typename... TArgs>
            concept OneOfTypes = (std::is_same_v<T, TArgs> || ...);
        }

        // До C++20: можно было забыть вернуть значение или вернуть не тот тип
        auto NothingReturn()
        {
            //return 0; // Если забыли написать return, то компилятор не выдаст ошибку
        }

        void DrawShape(const concepts::Shape auto& shape)
        {
            shape.Draw();
        }
    
        // C++20, 1 Способ: точно возвращается значение и нужный тип
        concepts::Shape auto GetShape1()
        {
            Point point;
            DerivedPoint derivedPoint;
            NoDerivedPoint noDerivedPoint;

            return point;
            //return derivedPoint; // OK
            //return noDerivedPoint; // ERROR
        }

        // C++20, 2 Способ: точно возвращается значение и нужный тип
        auto GetShape2() -> concepts::Shape auto
        {
            Point point;
            DerivedPoint derivedPoint;
            NoDerivedPoint noDerivedPoint;

            return point;
            //return derivedPoint; // OK
            //return noDerivedPoint; // ERROR
        }

            auto ABS(const concepts::OneOfTypes<int, double> auto& value)
        {
            return value < 0 ? -value : value;
        }

        auto GetSize(const auto& x)
        {
            if constexpr (concepts::hasSize<decltype(x)>)
                return x.size();
            else
                return x;
        }

        void Print(const auto& value)
        {
            std::cout << "Число: " << value << std::endl;
        }
    }

    namespace metafunction
    {
        namespace concepts
        {
            template<class T>
            concept isContainer = std::ranges::common_range<T>; // Условие: является ли тип контейнером

            template<typename T>
            concept hasData = requires (T & container)
            {
                container.data();
            }; // Условие: имеет ли тип метод data

            template<typename T>
            concept hasReserve = requires (T & container) // c const почему-то не работает
            {
                container.reserve(1);
            }; // Условие: является ли тип контейнером и имеет метод reserve

            template<typename T>
            concept isVector = isContainer<T> && hasData<T> && hasReserve<T>; // Условие: является ли тип std::vector

            template<typename T>
            concept isArray = isContainer<T> && hasData<T> && !hasReserve<T>; // Условие: является ли тип std::array
        }

        template<typename T>
        struct Info
        {
            static constexpr std::string_view type = "Unknown";
        };

        template<concepts::isContainer T>
        struct Info<T>
        {
            static constexpr std::string_view type = "Container";
        };

        template<concepts::isVector T>
        struct Info<T>
        {
            static constexpr std::string_view type = "Vector";
        };

        template<concepts::isArray T>
        struct Info<T>
        {
            static constexpr std::string_view type = "Array";
        };
    }
}

#endif /* Concept_h */
