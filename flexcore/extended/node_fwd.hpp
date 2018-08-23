#ifndef SRC_EXTENDED_NODE_FWD_HPP_
#define SRC_EXTENDED_NODE_FWD_HPP_

#include "extended/graph/graph.hpp"
#include "scheduler/parallelregion.hpp"

#include <memory>
#include <string>

namespace fc
{
/**
 * \brief Interface for all nodes (whether part of forest+graph or only graph)
 *
 * This interface is required to construct node_aware_mixin ports.
 */
class node
{
public:
	virtual ~node() = default;
	virtual graph::graph_node_properties graph_info() const = 0;
	virtual graph::connection_graph& get_graph() = 0;
	virtual std::shared_ptr<parallel_region> region() = 0;
	virtual std::string name() const = 0;
};
} // namespace fc

#endif
