#include "cron_parser.h"

#define EXPECT_THROW(f, E) try { \
	f; \
    assert(false); \
} \
catch (const E&) \
{} 

template<typename T = std::set<int>>
std::string create_mock_cron_output(const T & mins, const T & hours,
	const T & days, const T & months, const T & weekdays, const std::string & command)
{
	std::ostringstream oss;
	oss << std::left << std::setw(14) << "minute" << utils::details::join(mins) << std::endl;
	oss << std::left << std::setw(14) << "hour" << utils::details::join(hours) << std::endl;
	oss << std::left << std::setw(14) << "day of month" << utils::details::join(days) << std::endl;
	oss << std::left << std::setw(14) << "month" << utils::details::join(months) << std::endl;
	oss << std::left << std::setw(14) << "day of week" << utils::details::join(weekdays) << std::endl;
	oss << std::left << std::setw(14) << "command" << command << std::endl;
	return oss.str();
}

std::set<int> generate_set(int begin, int end)
{
	std::set<int> res;
	for (int i = begin; i <= end; ++i)
	{
		res.insert(i);
	}
	return res;
}

void run_tests()
{
	try
	{
		using namespace utils;
		assert(create_mock_cron_output({ 1 }, { 1 }, { 1 }, { 1 }, { 1 }, "foo") == parse("1 1 1 1 1 foo"));
		EXPECT_THROW(parse("1 1 1 foo"), std::exception);
		EXPECT_THROW(parse("1 1 1 1 1 1 foo"), std::exception);
		EXPECT_THROW(parse("1 1 1 7-6 1 foo"), std::exception);
		EXPECT_THROW(parse("1 1 1 13-6 1 foo"), std::exception);
		assert(create_mock_cron_output(generate_set(1, 59), { 1 }, { 1 }, { 1 }, { 1 }, "foo") == parse("1/1 1 1 1 1 foo"));
		assert(create_mock_cron_output(generate_set(0, 59), { 1 }, { 1 }, { 1 }, { 1 }, "foo") == parse("1,15,7,* 1 1 1 1 foo"));
		assert(create_mock_cron_output(generate_set(0, 59), { 1 }, { 1 }, { 1 }, { 1 }, "foo") == parse("1,15-22,7,* 1 1 1 1 foo"));
		EXPECT_THROW(parse("1,22-15,7,* 1 1 1 1 foo"), std::exception);
		assert(create_mock_cron_output(generate_set(0, 59), { 1 }, { 1 }, generate_set(1, 4), { 1 }, "foo") == parse("1,15-22,7,* 1 1 Mon-Thu,2,3 1 foo"));
		assert(create_mock_cron_output(generate_set(1, 27), { 1 }, { 1 }, { 1 }, { 1 }, "foo") == parse("1-27 1 1 1 1 foo"));
		assert(create_mock_cron_output(generate_set(1, 4), { 1 }, generate_set(12, 15), generate_set(1, 7), generate_set(1, 2), "foo") ==
			parse("1-4 1 12-15 JAN-JUL mon-tue foo"));
	}
	catch (const std::exception & e)
	{
		std::cerr << "Tests failed due to exception being thrown. Details " << e.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	// uncomment to run tests
	//run_tests();
	try
	{
		std::ostringstream oss;
		for (int i = 1; i < argc; ++i)
		{
			oss << argv[i];
			if (i != argc - 1)
			{
				oss << " ";
			}
		}
		std::cout << utils::parse(oss.str());
	}
	catch (const std::exception & e)
	{
		std::cerr << "Program finished due to exception being thrown. Details = " << e.what();
		return -1;
	}
	return 0;
}