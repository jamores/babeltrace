#ifndef BABELTRACE2_GRAPH_SELF_COMPONENT_PORT_H
#define BABELTRACE2_GRAPH_SELF_COMPONENT_PORT_H

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

#include <babeltrace2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline
const bt_port *bt_self_component_port_as_port(
		bt_self_component_port *self_port)
{
	return __BT_UPCAST_CONST(bt_port, self_port);
}

extern bt_self_component *bt_self_component_port_borrow_component(
		bt_self_component_port *self_port);

extern void *bt_self_component_port_get_data(
		const bt_self_component_port *self_port);

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_GRAPH_SELF_COMPONENT_PORT_H */
