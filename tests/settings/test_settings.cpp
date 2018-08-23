#include <boost/test/unit_test.hpp>

#include "utils/settings/settings.hpp"
#include "utils/settings/jsonfile_setting_backend.hpp"

using namespace fc;

BOOST_AUTO_TEST_SUITE(test_settings)

BOOST_AUTO_TEST_CASE(test_trivial_setting)
{
	int default_value = 0;

	const_setting_backend_facade facade{};

	setting<int> my_setting =
			{setting_id{"setting_id"},facade, default_value};

	BOOST_CHECK_EQUAL(my_setting(), default_value);
}

BOOST_AUTO_TEST_CASE(test_setting_json_file_good)
{
	std::stringstream ss;
	ss << "{ "
			"\"test_int\": 1,"
			"\"test_float\": 0.5 "
			"}";

	BOOST_CHECK_NO_THROW(json_file_setting_facade backend{ss};);
}

BOOST_AUTO_TEST_CASE(test_setting_json_file_bad)
{
	std::stringstream ss;
	ss << "{ "
			"\"test_int\": 1,"
			"\"test_float\": \"incompatible value"
			"}";

	BOOST_CHECK_THROW(json_file_setting_facade backend{ss}, cereal::Exception);
}

BOOST_AUTO_TEST_CASE(test_setting_from_json_file)
{
	int default_value = 0;

	std::stringstream ss;
	ss << "{ "
			"\"test_int\": 1,"
			"\"test_float\": 0.5 "
			"}";

	json_file_setting_facade backend{ss};

	setting<int> int_setting =
			{setting_id{"test_int"}, backend, default_value};

	//expect value from stream and not default value, since stream was loaded.
	BOOST_CHECK_EQUAL(int_setting(), 1);

	setting<float> float_setting =
			{setting_id{"test_float"}, backend, 0.0};
	BOOST_CHECK_EQUAL(float_setting(), 0.5);
}

BOOST_AUTO_TEST_CASE(test_incorrect_data_type)
{
	std::stringstream ss;
	ss << "{ "
			"\"test_int\": 1,"
			"\"test_float\": 0.5 "
			"}";

	json_file_setting_facade backend{ss};

	//try to read string setting from float value
	auto generate_illegal_setting = [&backend]()
	{
		setting<std::string> float_setting =
				{setting_id{"test_float"}, backend, "blabla"};

		return std::move(float_setting);
	};

	BOOST_CHECK_THROW(generate_illegal_setting(),cereal::Exception);
}

BOOST_AUTO_TEST_SUITE_END()
