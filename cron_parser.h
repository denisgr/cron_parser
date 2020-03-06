#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <exception>
#include <cassert>
#include <iomanip>
#include <set>
#include <numeric>

namespace utils
{

	namespace details
	{

		using UintSet = std::set<size_t>;
		using StringSet = std::set<std::string>;
		using String2IntMap = std::unordered_map<std::string, size_t>;
		using VectorStr = std::vector<std::string>;

		enum class expression_type
		{
			number, // e.g. 234
			numbers_range, // eg. 1-23
			step, // e.g. 0/15
			month_range, // JAN-DEC
			week_day_range, // MON-TUE
			asterisk // all values
		};

		class MinMaxPair
		{
		public:
			size_t begin;
			size_t end;

			MinMaxPair(size_t begin, size_t end)
				: begin(begin)
				, end(end)
			{}
		};

		class TimeField
		{
			std::string name;
			MinMaxPair minMaxRange;
		public:
			TimeField(const std::string& name, MinMaxPair minMaxPair)
				: name(name)
				, minMaxRange(minMaxPair)
			{}

			const std::string& GetName() const { return name; }
			const MinMaxPair& GetRange() const { return minMaxRange; }
		};

		class Month
		{
		public:
			static size_t GetMonthNumber(const std::string& m)
			{
				static const auto& m2nMap = getMonthName2NumberMap();
				auto number = m2nMap.find(m);
				if (number != m2nMap.end())
				{
					return number->second;
				}
				std::ostringstream oss;
				oss << "Given month " << m << " does not exist.";
				throw std::exception(oss.str().c_str());
			}

			static const StringSet& GetMonthNames()
			{
				static auto monthNames = getMonthNames();
				return monthNames;
			}

		private:
			static const String2IntMap& getMonthName2NumberMap()
			{
				static String2IntMap monthName2IntMap = { {"JAN", 1},
				{ "FEB", 2 }, { "MAR", 3 }, { "APR", 4 }, { "MAY", 5 }, { "JUN", 6 }, { "JUL", 7 },
				{ "AUG", 8 }, { "SEP", 9 }, { "OCT", 10 }, { "NOV", 11 }, { "DEC", 12 } };
				return monthName2IntMap;
			}

			static StringSet getMonthNames()
			{
				const auto& monthName2IntMap = getMonthName2NumberMap();
				StringSet monthNames;
				for (auto&& item : monthName2IntMap)
				{
					monthNames.insert(item.first);
				}
				return monthNames;
			}
		};

		class WeekDay
		{
		public:
			static size_t GetWeekDayNumber(const std::string& str)
			{
				auto res = getWeekDayName2IntegerMap();
				auto weekday = res.find(str);
				if (res.end() != weekday)
				{
					return weekday->second;
				}
				std::ostringstream oss;
				oss << "Weekday " << str << " does not exist.";
				throw std::exception(oss.str().c_str());
			}

			static const StringSet& GetWeekDaysNames()
			{
				static auto weekDayNames = getWeekDaysNames();
				return weekDayNames;
			}
		private:
			static const String2IntMap& getWeekDayName2IntegerMap()
			{
				static String2IntMap weekDayName2Integer = { {"MON", 1}, {"TUE", 2}, {"WED", 3},
					{"THU", 4}, {"FRI", 5}, {"SAT", 6}, {"SUN", 7} };
				return weekDayName2Integer;
			}

			static const StringSet getWeekDaysNames()
			{
				const auto& weekDays = getWeekDayName2IntegerMap();
				StringSet ss;
				for (auto&& w : weekDays)
				{
					ss.insert(w.first);
				}
				return ss;
			}
		};

		void to_upper(std::string& str)
		{
			for (auto& c : str) c = toupper(c);
		}

		template<typename T>
		std::string join_str(const T & cont, char sep = ' ')
		{
			std::ostringstream imploded;
			std::copy(cont.begin(), cont.end(),
				std::ostream_iterator<std::string>(imploded, &sep));
			auto res = imploded.str().substr(0, imploded.str().size() - 1);
			return res;
		}

		template<typename T>
		std::string join(const T & cont, char sep = ' ')
		{
			std::vector<std::string> strings(cont.size());
			std::transform(cont.begin(), cont.end(), strings.begin(), [](const typename T::value_type & t) {return std::to_string(t); });
			return join_str(strings, sep);
		}

		template<typename T>
		void split(const std::string & str, T & cont, char sep = ' ')
		{
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, sep)) {
				cont.push_back(token);
			}
		}

		UintSet get_numbers_for_time_field(expression_type et, const std::string & str, const MinMaxPair & minMaxPair)
		{
			switch (et)
			{
			case expression_type::asterisk:
			{
				UintSet res;
				for (size_t i = minMaxPair.begin; i <= minMaxPair.end; ++i)
				{
					res.insert(i);
				}
				return res;
			}
			case expression_type::number:
			{
				return UintSet({ static_cast<size_t>(std::stoi(str)) });
			}
			case expression_type::numbers_range:
			{
				VectorStr numbers_range;
				split(str, numbers_range, '-');
				// no need to check size of numbers_range since regexp guarantees return correct range
				size_t begin = std::stoi(numbers_range[0]);
				size_t end = std::stoi(numbers_range[1]);
				if (begin > end)
				{
					std::ostringstream oss;
					oss << "Wrong range. Begin number(" << begin << ") is greater than end number(" << end << ").";
					throw std::exception(oss.str().c_str());
				}
				UintSet result;

				for (size_t i = begin; i <= end; ++i)
				{
					result.insert(i);
				}
				return result;
			}
			case expression_type::step:
			{
				VectorStr args;
				split(str, args, '/');

				UintSet result;
				size_t begin = args[0] == "*" ? 0 : std::stoi(args[0]);
				size_t step = std::stoi(args[1]);
				for (auto i = begin; i <= minMaxPair.end; i += step)
					result.insert(i);

				return result;
			}
			case expression_type::month_range:
			{
				VectorStr args;
				split(str, args, '-');

				size_t begin = Month::GetMonthNumber(args[0]);
				size_t end = Month::GetMonthNumber(args[1]);

				if (begin > end)
				{
					std::ostringstream oss;
					oss << "Provided wrong months range " << str << ".";
					throw std::exception(oss.str().c_str());
				}
				UintSet result;
				for (size_t i = begin; i <= end; ++i)
				{
					result.insert(i);
				}
				return result;
			}
			case expression_type::week_day_range:
			{
				VectorStr args;
				split(str, args, '-');

				size_t begin = WeekDay::GetWeekDayNumber(args[0]);
				size_t end = WeekDay::GetWeekDayNumber(args[1]);

				if (begin > end)
				{
					std::ostringstream oss;
					oss << "Provided wrong week days range " << str << ".";
					throw std::exception(oss.str().c_str());
				}
				UintSet result;
				for (size_t i = begin; i <= end; ++i)
				{
					result.insert(i);
				}
				return result;
			}
			default:
				break;
			}
			return UintSet();
		}

		UintSet parse_time_field(const std::string & str, const MinMaxPair & minMaxPair)
		{
			VectorStr list;
			split(str, list, ',');

			std::ostringstream mnoss, wdoss;
			mnoss << "(" << join_str(Month::GetMonthNames(), '|') << ")-(" << join_str(Month::GetMonthNames(), '|') << ")";
			wdoss << "(" << join_str(WeekDay::GetWeekDaysNames(), '|') << ")-(" << join_str(WeekDay::GetWeekDaysNames(), '|') << ")";

			static std::unordered_map<expression_type, std::regex> expressions({
				{expression_type::asterisk, std::regex("\\*") },
				{expression_type::number, std::regex("\\d+") },
				{expression_type::step, std::regex("(\\*|\\d+)\\/(\\*|\\d+)")},
				{expression_type::numbers_range, std::regex("\\d+-\\d+") },
				{expression_type::month_range, std::regex(mnoss.str())},
				{expression_type::week_day_range, std::regex(wdoss.str()) }
				});

			UintSet result;
			for (auto&& list_item : list)
			{
				std::smatch m;
				bool parsed_successfully = false;
				expression_type et;
				for (auto&& e : expressions)
				{
					if (std::regex_match(list_item, m, e.second))
					{
						parsed_successfully = true;
						et = e.first;
						break;
					}
				}

				if (!parsed_successfully)
				{
					std::ostringstream oss;
					oss << "Unable to parse '" << str << "' time field";
					throw std::exception(oss.str().c_str());
				}

				auto res = get_numbers_for_time_field(et, list_item, minMaxPair);
				result.insert(res.begin(), res.end());
			}
			return result;
		}

	}

	/// @brief parses cron input and returns string that has unwrapped time fields
	/// @param input - cron input
	std::string parse(const std::string & input)
	{
		using namespace details;
		const size_t offset = 14;
		std::vector<std::string> args, time_fields;
		std::string command;
		split(input, args);
		const size_t amount_of_args = 6;
		if (args.size() != amount_of_args)
		{
			std::ostringstream oss;
			oss << "Incorrect command. Expected " << amount_of_args << " arguments.";
			throw std::exception(oss.str().c_str());
		}

		std::copy(args.begin(), args.end() - 1, std::back_inserter(time_fields));
		std::transform(time_fields.begin(), time_fields.end(), time_fields.begin(), [](std::string & str) { to_upper(str); return str; });
		command = args.back();

		std::vector<TimeField> time_fields_ranges = { TimeField("minute", MinMaxPair(0, 59)),
			TimeField("hour", MinMaxPair(0,23)),
			TimeField("day of month", MinMaxPair(1,31)),
			TimeField("month", MinMaxPair(1,12)),
			TimeField("day of week", MinMaxPair(1,7)) };

		auto ctf = time_fields_ranges.cbegin();
		std::ostringstream oss;
		for (auto&& tf : time_fields)
		{
			auto res = parse_time_field(tf, ctf->GetRange());
			oss << std::left << std::setw(offset) << ctf->GetName() << join(res) << std::endl;
			++ctf;
		}
		oss << std::left << std::setw(offset) << "command" << command << std::endl;

		return oss.str();
	}

}