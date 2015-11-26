#include <boost/test/unit_test.hpp>

#include "core/connection.hpp"
#include "ports/event_ports.hpp"
#include <vector>
#include <algorithm>
#include "move_token.hpp"

using namespace fc;

BOOST_AUTO_TEST_SUITE(test_tokens_testing)

BOOST_AUTO_TEST_CASE( move_token_ )
{
	// no default constructor
	// how to test for "does not compile"?
//	move_token t;

	// non-copyable
	move_token t("foo");
	move_token u(move_token("bar"));

	std::vector<move_token> v(10);
	std::sort(v.begin(), v.end());
	v.push_back(move_token("foo"));
}

BOOST_AUTO_TEST_CASE( moving )
{
#warning re-enable test!
//	auto set_bar = [](move_token&& t) -> move_token&& { t.value() = "bar"; return std::move(t); };
////	auto set_bar = [](move_token&& t) { t.value() = "bar"; return std::move(t); };
//
//	event_out_port<move_token> source;
//	event_in_queue<move_token> sink;
//	source >> set_bar >> sink;
//	source.fire(move_token("foo"));
//	move_token received(sink.get());
//	BOOST_CHECK_EQUAL(received.value(), std::string("bar"));
}

BOOST_AUTO_TEST_SUITE_END()






