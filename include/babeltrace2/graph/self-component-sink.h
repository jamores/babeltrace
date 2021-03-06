#ifndef BABELTRACE2_GRAPH_SELF_COMPONENT_SINK_H
#define BABELTRACE2_GRAPH_SELF_COMPONENT_SINK_H

/*
 * Copyright (c) 2010-2019 EfficiOS Inc. and Linux Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __BT_IN_BABELTRACE_H
# error "Please include <babeltrace2/babeltrace.h> instead."
#endif

#include <stdint.h>

#include <babeltrace2/graph/self-component.h>
#include <babeltrace2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
bt_self_component *bt_self_component_sink_as_self_component(
		bt_self_component_sink *self_comp_sink)
{
	return __BT_UPCAST(bt_self_component, self_comp_sink);
}

static inline
const bt_component_sink *
bt_self_component_sink_as_component_sink(
		bt_self_component_sink *self_comp_sink)
{
	return __BT_UPCAST_CONST(bt_component_sink, self_comp_sink);
}

extern bt_self_component_port_input *
bt_self_component_sink_borrow_input_port_by_name(
		bt_self_component_sink *self_component,
		const char *name);

extern bt_self_component_port_input *
bt_self_component_sink_borrow_input_port_by_index(
		bt_self_component_sink *self_component, uint64_t index);

extern bt_self_component_add_port_status
bt_self_component_sink_add_input_port(
		bt_self_component_sink *self_component,
		const char *name, void *user_data,
		bt_self_component_port_input **self_component_port);

extern bt_bool bt_self_component_sink_is_interrupted(
		const bt_self_component_sink *self_component);

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_GRAPH_SELF_COMPONENT_SINK_H */
