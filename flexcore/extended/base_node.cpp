#include "extended/base_node.hpp"

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include "extended/visualization/visualization.hpp"

#include <stack>

namespace fc
{

static forest_t::iterator find_self(forest_t& forest, const tree_node& node)
{
	auto node_id = node.graph_info().get_id();
	auto self = std::find_if(forest.begin(), forest.end(),
			[=](auto& other_uniq_ptr) { return node_id == other_uniq_ptr->graph_info().get_id(); });
	assert(self != forest.end());
	return adobe::trailing_of(self);
}

std::string full_name(forest_t& forest, const tree_node& node)
{
	auto position = find_self(forest, node);
	// push names of parent / grandparent ... to stack to later reverse order.
	std::stack<std::string> name_stack;
	for (auto parent = adobe::find_parent(position); parent != forest.end();
			parent = adobe::find_parent(parent))
	{
		name_stack.emplace((*parent)->name());
	}
	std::string full_name;
	while (!name_stack.empty())
	{
		full_name += (name_stack.top() + name_seperator);
		name_stack.pop();
	}
	full_name += (*position)->name();
	return full_name;
}

tree_base_node::tree_base_node(const node_args& args)
	: fg_(args.fg), region_(args.r), graph_info_(args.graph_info)
{
	assert(region_);
}

std::string tree_base_node::name() const
{
	return graph_info_.name();
}

graph::graph_node_properties tree_base_node::graph_info() const
{
	return graph_info_;
}

graph::connection_graph& tree_base_node::get_graph()
{
	return fg_.graph;
}

forest_t::iterator owning_base_node::self() const
{
	return self_;
}

forest_t::iterator owning_base_node::add_child(std::unique_ptr<tree_node> child)
{
	assert(child);
	auto& forest = fg_.forest;
	auto child_it = adobe::trailing_of(forest.insert(self(), std::move(child)));
	assert(adobe::find_parent(child_it) == self());
	assert(adobe::find_parent(child_it) != forest.end());
	return child_it;
}

node_args owning_base_node::new_node(node_args args)
{
	const auto proxy_iter = add_child(std::make_unique<tree_base_node>(args));
	args.self = proxy_iter;
	return args;
}

forest_owner::forest_owner(
		graph::connection_graph& graph, std::string n, std::shared_ptr<parallel_region> r)
	: fg_(std::make_unique<forest_graph>(graph))
	, tree_root(nullptr)
	, viz_(std::make_unique<visualization>(fg_->graph, fg_->forest))
{
	assert(r);
	assert(fg_);
	auto& forest = fg_->forest;
	auto args = node_args{*fg_,  std::move(r),  std::move(n)};
	//first place a proxy node in the forest to create tree_node
	const auto iter = adobe::trailing_of(
			forest.insert(forest.begin(), std::make_unique<tree_base_node>(args)));
	args.self = iter;

	// replace proxy with actual node
	*iter = std::make_unique<owning_base_node>(args);

	tree_root = dynamic_cast<owning_base_node*>(iter->get());
	assert(tree_root);
	assert(fg_);
	assert(viz_);
}

forest_owner::~forest_owner()
{
}

void forest_owner::visualize(std::ostream& out) const
{
	assert(viz_);
	viz_->visualize(out);
}
}
