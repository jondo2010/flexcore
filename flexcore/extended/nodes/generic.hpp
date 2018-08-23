#ifndef SRC_NODES_GENERIC_HPP_
#define SRC_NODES_GENERIC_HPP_

#include "core/traits.hpp"
#include "extended/ports/token_tags.hpp"
#include "pure/pure_ports.hpp"
#include "pure/pure_node.hpp"
#include "extended/base_node.hpp"
#include "extended/nodes/region_worker_node.hpp"

#include <utility>
#include <map>

namespace fc
{

/**
 * \brief n_ary_switch forwards one of n inputs to output
 *
 * Simply connect new input ports to add them to the set for the switch.
 * The switch itself is controlled by the port "control" which needs to be connected
 * to a state source of a type convertible to key_t.
 * is specialized for state and events, as the implementations differ.
 *
 * \tparam data_t type of data flowing through the switch
 * \tparam tag either event_tag or state_tag to set switch to event handling
 * or forwarding of state
 *
 * \tparam key_t key for lookup of inputs in switch. needs to have operator < and ==
 * \ingroup nodes
 */
template<class data_t,
		class tag,
		class key_t = size_t,
		class base_node = tree_base_node
		> class n_ary_switch;

template<class data_t, class key_t, class base_node>
class n_ary_switch<data_t, state_tag, key_t, base_node> : public base_node
{
public:
	template<class... base_args>
	explicit n_ary_switch(base_args&&... args)
		: base_node(std::forward<base_args>(args)...)
		, switch_state(this)
		, in_ports()
		, out_port(this, [this](){return in_ports.at(switch_state.get()).get();} )
	{}

	using data_sink_t = typename base_node::template state_sink<data_t>;
	using key_sink_t = typename base_node::template state_sink<key_t>;
	using state_source_t = typename base_node::template state_source<data_t>;

	/**
	 * \brief input port for state of type data_t corresponding to key port.
	 *
	 * \returns input port corresponding to key
	 * \param port key by which port is identified.
	 * \post !in_ports.empty()
	 */
	auto& in(key_t port) noexcept
	{
		auto it = in_ports.find(port);
		if (it == in_ports.end())
			it = in_ports.emplace(std::make_pair(port,
					data_sink_t{this})).first;
		return it->second;
	}
	/// parameter port controlling the switch, expects state of key_t
	auto& control() noexcept { return switch_state; }
	auto& out() noexcept { return out_port; }
private:
	/// provides the current state of the switch.
	key_sink_t switch_state;
	std::map<key_t, data_sink_t> in_ports;
	state_source_t out_port;
};

/// partial specialization of n_ary_switch for events
template<class data_t, class key_t, class base_node>
class n_ary_switch<data_t, event_tag, key_t, base_node> : public base_node
{
public:
	using data_sink_t = typename base_node::template event_sink<data_t>;
	using key_sink_t = typename base_node::template state_sink<key_t>;
	using event_source_t = typename base_node::template event_source<data_t>;


	template<class... base_args>
	explicit n_ary_switch(base_args&&... args)
		: base_node(std::forward<base_args>(args)...)
		, switch_state(this)
		, out_port(this)
		, in_ports()
	{}

	/**
	 * \brief Get port by key. Creates port if none was found for key.
	 *
	 * \returns input port corresponding to key
	 * \param port key by which port is identified.
	 * \post !in_ports.empty()
	 */
	auto& in(key_t port)
	{
		auto it = in_ports.find(port);
		if (it == end(in_ports))
		{
			it = in_ports.emplace(std::make_pair
				(	port,
					data_sink_t( this,
							[this, port](const data_t& in){ forward_call(in, port); })
				)
			).first;
		} //else the port already exists, we can just return it

		return it->second;
	}

	/// output port of events of type data_t.
	auto& out() noexcept { return out_port; }
	/// parameter port controlling the switch, expects state of key_t
	auto& control() noexcept { return switch_state; }


private:
	key_sink_t switch_state;
	event_source_t out_port;
	std::map<key_t, data_sink_t> in_ports;
	/// fires incoming event if and only if it is from the currently chosen port.
	void forward_call(data_t event, key_t port)
	{
		assert(!in_ports.empty());
		assert(in_ports.find(port) != end(in_ports));

		if (port == switch_state.get())
			out().fire(event);
	}
};

/**
 * \brief node which observes a state and fires an event if the state matches a predicate.
 *
 * Needs to be connected to a tick, which triggers the check of the predicate on the state.
 *
 * \tparam data_t type of data watched by the watch_node.
 * \tparam predicate predicate which is tested on the observed state
 * predicate needs to be a callable which takes objects convertible from data_t
 * and returns a bool.
 * \ingroup nodes
 */
template<class data_t, class predicate, class base_node>
class watch_node : public base_node
{
public:
	template<class... base_args>
	explicit watch_node(predicate pred, base_args&&... args)
		: base_node(std::forward<base_args>(args)...)
		, pred{std::move(pred)}
		, in_port(this)
		, out_port(this)
	{
	}

	watch_node(watch_node&&) = default;

	/// State input port, expects data_t.
	auto& in() noexcept { return in_port; }
	/// Event Output port, fires data_t.
	auto& out() noexcept { return out_port; }

	/// Event input port expects event of type void. Usually connected to a work_tick.
	auto check_tick()
	{
		return [this]()
		{
			const auto tmp = in_port.get();
			if (pred(tmp))
				out_port.fire(tmp);
		};
	}

private:
	predicate pred;
	typename base_node::template state_sink<data_t> in_port;
	typename base_node::template event_source<data_t> out_port;
};

/**
 * \brief Creates a watch node with a predicate.
 * \param pred predicate which is tested on the observed state
 * \return watch_node with the given predicate
 * \ingroup nodes
 */
template<class data_t, class predicate>
auto watch(predicate&& pred, data_t)
{
	return watch_node<data_t, predicate, pure::pure_node>{std::forward<predicate>(pred)};
}

/**
 * \brief Creates a watch_node, which fires an event, if the state changes.
 *
 *  Does not fire the first time the state is querried.
 *  \ingroup nodes
 */
template<class data_t>
auto on_changed(data_t initial_value = data_t())
{
	return watch(
			[last = std::make_unique<data_t>()](const data_t& in) mutable
			{
				const bool is_same = last && (*last == in);
				last = std::make_unique<data_t>(in);
				return !is_same;
			},
			initial_value);
}

/**
 * \brief Variant of watch_node which is attached to a region
 * \tparam data_t type of event forwarded by this node
 * \tparam predicate a unary predicate which accepts data_t and returns bool
 * Data is forwarded for every token for which predicate returns true.
 */
template<class data_t, class predicate = std::function<bool(data_t)>>
class unary_watch_node final : public fc::region_worker_node
{
public:
	/// Constructor taking a predicate by value and storing it
	explicit unary_watch_node(predicate p, const node_args& args)
		: region_worker_node([this]()
				{
					const auto tmp = this->in_port.get();
					if (this->pred(tmp))
						this->out_port.fire(tmp);
				},
				args)
		, pred{std::move(p)}
		, in_port{this}
		, out_port{this}
	{
	}

	/// State input port, expects data_t.
	auto& in() noexcept { return in_port; }
	/// Event Output port, fires data_t.
	auto& out() noexcept { return out_port; }

	unary_watch_node(const unary_watch_node&) = delete;
	unary_watch_node(unary_watch_node&&) = delete;

private:
	predicate pred;
	fc::state_sink<data_t> in_port;
	fc::event_source<data_t> out_port;

};

}  // namespace fc

#endif /* SRC_NODES_GENERIC_HPP_ */
