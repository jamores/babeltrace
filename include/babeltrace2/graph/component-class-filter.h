#ifndef BABELTRACE2_GRAPH_COMPONENT_CLASS_FILTER_H
#define BABELTRACE2_GRAPH_COMPONENT_CLASS_FILTER_H

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

#include <babeltrace2/graph/component-class.h>
#include <babeltrace2/types.h>
#include <babeltrace2/logging.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bt_component_class_get_supported_mip_versions_method_status
(*bt_component_class_filter_get_supported_mip_versions_method)(
		bt_self_component_class_filter *comp_class,
		const bt_value *params, void *init_method_data,
		bt_logging_level log_level,
		bt_integer_range_set_unsigned *supported_versions);

typedef bt_component_class_initialize_method_status
(*bt_component_class_filter_initialize_method)(
		bt_self_component_filter *self_component,
		bt_self_component_filter_configuration *config,
		const bt_value *params, void *init_method_data);

typedef void (*bt_component_class_filter_finalize_method)(
		bt_self_component_filter *self_component);

typedef bt_component_class_query_method_status
(*bt_component_class_filter_query_method)(
		bt_self_component_class_filter *comp_class,
		bt_private_query_executor *query_executor,
		const char *object, const bt_value *params,
		void *method_data, const bt_value **result);

typedef bt_component_class_port_connected_method_status
(*bt_component_class_filter_input_port_connected_method)(
		bt_self_component_filter *self_component,
		bt_self_component_port_input *self_port,
		const bt_port_output *other_port);

typedef bt_component_class_port_connected_method_status
(*bt_component_class_filter_output_port_connected_method)(
		bt_self_component_filter *self_component,
		bt_self_component_port_output *self_port,
		const bt_port_input *other_port);

static inline
bt_component_class *bt_component_class_filter_as_component_class(
		bt_component_class_filter *comp_cls_filter)
{
	return __BT_UPCAST(bt_component_class, comp_cls_filter);
}

extern
bt_component_class_filter *bt_component_class_filter_create(
		const char *name,
		bt_message_iterator_class *message_iterator_class);

extern bt_component_class_set_method_status
bt_component_class_filter_set_get_supported_mip_versions_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_get_supported_mip_versions_method method);

extern bt_component_class_set_method_status
bt_component_class_filter_set_initialize_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_initialize_method method);

extern bt_component_class_set_method_status
bt_component_class_filter_set_finalize_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_finalize_method method);

extern bt_component_class_set_method_status
bt_component_class_filter_set_input_port_connected_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_input_port_connected_method method);

extern bt_component_class_set_method_status
bt_component_class_filter_set_output_port_connected_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_output_port_connected_method method);

extern bt_component_class_set_method_status
bt_component_class_filter_set_query_method(
		bt_component_class_filter *comp_class,
		bt_component_class_filter_query_method method);

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_GRAPH_COMPONENT_CLASS_FILTER_H */
