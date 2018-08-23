#include <boost/test/unit_test.hpp>

#include "extended/base_node.hpp"

#include "../../nodes/owning_node.hpp"

// std
#include <memory>

using namespace fc;

namespace // unnamed
{
template<class data_t>
struct node_class : tree_base_node
{
	static constexpr auto default_name = "test_node";
	node_class(data_t a, const node_args& node)
		: tree_base_node(node)
		, value(a)
	{}

	data_t get_value() { return value; }

	auto port()
	{
		return [this](){ return this->get_value(); };
	}

	data_t value;
};

struct null : tree_base_node
{
	explicit null(const node_args& node) : tree_base_node(node) {}
};
} // unnamed namespace

BOOST_AUTO_TEST_SUITE( test_base_node )

/*
 * Confirm region propagation to child
 */
BOOST_AUTO_TEST_CASE( test_region_propagation )
{
	auto region = std::make_shared<parallel_region>("foo",fc::thread::cycle_control::fast_tick);
	tests::owning_node root(region);
	auto& child = root.make_child_named<null>("child");

	BOOST_CHECK(child.region()->get_id() == region->get_id());
}

namespace
{
class test_owning_node : public owning_base_node
{
public:
	static constexpr auto default_name = "test_owning_node_";
	/*
	explicit test_owning_node(forest_t::iterator self, forest_graph* fg, std::shared_ptr<parallel_region> r)
	    : owning_base_node(self, fg, r, default_name)
	{
	}
	*/
	explicit test_owning_node(const node_args& node)
	    : owning_base_node(node)
	{
	}
	test_owning_node&  add_child()
	{
		return make_child_named<test_owning_node>(region(), default_name);
	}

	size_t nr_of_children()
	{
		size_t n = std::distance(++adobe::leading_of(self()), adobe::trailing_of(self()));
		return n / 2; // all nodes get visited twice
	}

	using owning_base_node::self;
};
}

/*
 * Confirm full names
 */
BOOST_AUTO_TEST_CASE( test_name_chaining )
{
	tests::owning_node root("root");
	auto& child1 =
	    root.node().make_child_named<test_owning_node>(root.node().region(), "test_owning_node");
	auto& child2 = root.make_child_named<null>("2");
	auto& child1a = child1.make_child_named<null>("a");

	BOOST_CHECK_EQUAL(full_name(*(root.forest()),child1), "root.test_owning_node");
	BOOST_CHECK_EQUAL(full_name(*(root.forest()),child2), "root.2");
	BOOST_CHECK_EQUAL(full_name(*(root.forest()),child1a), "root.test_owning_node.a");
}

BOOST_AUTO_TEST_CASE( test_make_child )
{
	tests::owning_node root("root");
	auto& child1 = root.make_child<node_class<int>>(5);
	auto& child2 = root.make_child_named<node_class<int>>("name", 5);

	BOOST_CHECK_EQUAL(full_name(*(root.forest()), child1), "root.test_node");
	BOOST_CHECK_EQUAL(full_name(*(root.forest()),child2), "root.name");
}

BOOST_AUTO_TEST_CASE( test_manual_ownership )
{
	tests::owning_node root("root");
	node_class<int> child1{5, root.new_node("test_node")};
	node_class<int> child2{5, root.new_node(root.region(), "name")};

	BOOST_CHECK_EQUAL(full_name(*(root.forest()), child1), "root.test_node");
	BOOST_CHECK_EQUAL(full_name(*(root.forest()),child2), "root.name");
}

BOOST_AUTO_TEST_CASE( test_deletion )
{
	tests::owning_node root_;
	auto& root = root_.node();

	auto& test_node =
	    root.make_child<test_owning_node>(root.region());

	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 0);

	auto& temp_it = test_node.add_child();
	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 1);

	erase_with_subtree(*(root_.forest()), temp_it.self());
	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 0);

	auto& temp_it_2 = test_node.add_child();
	temp_it_2.add_child();
	auto& temp_it_3 = test_node.add_child();

	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 3);
	erase_with_subtree(*(root_.forest()), temp_it_2.self());
	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 1);
	erase_with_subtree(*(root_.forest()), temp_it_3.self());

	BOOST_CHECK_EQUAL(test_node.nr_of_children(), 0);
}

namespace
{
template <class T>
class full_name_test_node : public T
{
public:
	full_name_test_node(const std::string& wanted_fullname, const fc::node_args& args)
	:  T(args)
	{
		BOOST_CHECK_EQUAL(wanted_fullname, fc::full_name(this->fg_.forest, *this));
	}
};
}

BOOST_AUTO_TEST_CASE( tree_base_node_can_get_full_name_in_constructor )
{
	tests::owning_node root_("root");
	auto& root = root_.node();
	// runs the test
	root.make_child_named<full_name_test_node<fc::tree_base_node>>("my_node", "root.my_node");
}

BOOST_AUTO_TEST_CASE( owning_base_node_can_get_full_name_in_constructor )
{
	tests::owning_node root_("root");
	auto& root = root_.node();
	// runs the test
	root.make_child_named<full_name_test_node<fc::owning_base_node>>("my_node", "root.my_node");
}

BOOST_AUTO_TEST_SUITE_END()
