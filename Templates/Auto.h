#ifndef Auto_h
#define Auto_h

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
	inline constexpr auto GetSum(const auto& value1, const auto& value2)
	{
		return value1 + value2;
	}

	// Унарная правоассоциативная свёртка
	inline constexpr auto Sum(auto&&... args)
	{
		return (args + ...); // (arg1+ arg2 + arg3 + ...)
	}

	// Унарная правоассоциативная свёртка
	inline constexpr auto Average(auto&&... args) // Сокращенный шаблон
	{
		auto s = Sum(args...);
		return s / sizeof...(args);
	}

	namespace CONCEPT
	{
		inline constexpr void Print_Strings(std::convertible_to<std::string_view> auto&& ...strings) // Сокращенный шаблон
		{
			for (const auto& s : std::initializer_list<std::string_view>{ std::forward<std::string_view>(strings)... })
				std::cout << s << ", ";
			std::cout << std::endl;
		}

		inline constexpr void Print(const std::ranges::common_range auto& container)
		{
			for (const auto& item : container)
				std::cout << item << ", ";
			std::cout << std::endl;
		}
	}
}

#endif
