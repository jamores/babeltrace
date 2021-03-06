#ifndef BABELTRACE2_TYPES_H
#define BABELTRACE2_TYPES_H

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

#ifdef __cplusplus
extern "C" {
#endif

/**
@defgroup ctypes Babeltrace C types
@ingroup apiref
@brief Babeltrace C types.

@code
#include <babeltrace2/types.h>
@endcode

This header contains custom type definitions used across the library.

@file
@brief Babeltrace C types.
@sa ctypes

@addtogroup ctypes
@{
*/

/// False boolean value for the #bt_bool type.
#define BT_FALSE	0

/// True boolean value for the #bt_bool type.
#define BT_TRUE		1

/**
@brief	Babeltrace's boolean type.

Use only the #BT_FALSE and #BT_TRUE definitions for #bt_bool parameters.
It is guaranteed that the library functions which return a #bt_bool
value return either #BT_FALSE or #BT_TRUE.

You can always test the truthness of a #bt_bool value directly, without
comparing it to #BT_TRUE directly:

@code
bt_bool ret = bt_some_function(...);

if (ret) {
	// ret is true
}
@endcode
*/
typedef int bt_bool;

typedef uint64_t bt_listener_id;

typedef const uint8_t *bt_uuid;

typedef struct bt_clock_class bt_clock_class;
typedef struct bt_clock_snapshot bt_clock_snapshot;
typedef struct bt_component bt_component;
typedef struct bt_component_class bt_component_class;
typedef struct bt_component_class_filter bt_component_class_filter;
typedef struct bt_component_class_sink bt_component_class_sink;
typedef struct bt_component_class_source bt_component_class_source;
typedef struct bt_component_descriptor_set bt_component_descriptor_set;
typedef struct bt_component_filter bt_component_filter;
typedef struct bt_component_sink bt_component_sink;
typedef struct bt_component_source bt_component_source;
typedef struct bt_connection bt_connection;
typedef struct bt_error bt_error;
typedef struct bt_error_cause bt_error_cause;
typedef struct bt_event bt_event;
typedef struct bt_event_class bt_event_class;
typedef struct bt_event_header_field bt_event_header_field;
typedef struct bt_field bt_field;
typedef struct bt_field_class bt_field_class;
typedef struct bt_field_class_enumeration_mapping bt_field_class_enumeration_mapping;
typedef struct bt_field_class_enumeration_signed_mapping bt_field_class_enumeration_signed_mapping;
typedef struct bt_field_class_enumeration_unsigned_mapping bt_field_class_enumeration_unsigned_mapping;
typedef struct bt_field_class_structure_member bt_field_class_structure_member;
typedef struct bt_field_class_variant_option bt_field_class_variant_option;
typedef struct bt_field_class_variant_with_selector_field_integer_signed_option bt_field_class_variant_with_selector_field_integer_signed_option;
typedef struct bt_field_class_variant_with_selector_field_integer_unsigned_option bt_field_class_variant_with_selector_field_integer_unsigned_option;
typedef struct bt_field_path bt_field_path;
typedef struct bt_field_path_item bt_field_path_item;
typedef struct bt_graph bt_graph;
typedef struct bt_integer_range_set bt_integer_range_set;
typedef struct bt_integer_range_set_signed bt_integer_range_set_signed;
typedef struct bt_integer_range_set_unsigned bt_integer_range_set_unsigned;
typedef struct bt_integer_range_signed bt_integer_range_signed;
typedef struct bt_integer_range_unsigned bt_integer_range_unsigned;
typedef struct bt_interrupter bt_interrupter;
typedef struct bt_message bt_message;
typedef struct bt_message_iterator bt_message_iterator;
typedef struct bt_message_iterator_class bt_message_iterator_class;
typedef struct bt_object bt_object;
typedef struct bt_packet bt_packet;
typedef struct bt_plugin bt_plugin;
typedef struct bt_plugin_set bt_plugin_set;
typedef struct bt_plugin_so_shared_lib_handle bt_plugin_so_shared_lib_handle;
typedef struct bt_port bt_port;
typedef struct bt_port_input bt_port_input;
typedef struct bt_port_output bt_port_output;
typedef struct bt_port_output_message_iterator bt_port_output_message_iterator;
typedef struct bt_private_query_executor bt_private_query_executor;
typedef struct bt_query_executor bt_query_executor;
typedef struct bt_self_component bt_self_component;
typedef struct bt_self_component_class bt_self_component_class;
typedef struct bt_self_component_class_filter bt_self_component_class_filter;
typedef struct bt_self_component_class_sink bt_self_component_class_sink;
typedef struct bt_self_component_class_source bt_self_component_class_source;
typedef struct bt_self_component_filter bt_self_component_filter;
typedef struct bt_self_component_filter_configuration bt_self_component_filter_configuration;
typedef struct bt_self_component_port bt_self_component_port;
typedef struct bt_self_component_port_input bt_self_component_port_input;
typedef struct bt_message_iterator bt_message_iterator;
typedef struct bt_self_component_port_output bt_self_component_port_output;
typedef struct bt_self_component_sink bt_self_component_sink;
typedef struct bt_self_component_sink_configuration bt_self_component_sink_configuration;
typedef struct bt_self_component_source bt_self_component_source;
typedef struct bt_self_component_source_configuration bt_self_component_source_configuration;
typedef struct bt_self_message_iterator bt_self_message_iterator;
typedef struct bt_self_message_iterator_configuration bt_self_message_iterator_configuration;
typedef struct bt_self_plugin bt_self_plugin;
typedef struct bt_stream bt_stream;
typedef struct bt_stream_class bt_stream_class;
typedef struct bt_trace bt_trace;
typedef struct bt_trace_class bt_trace_class;
typedef struct bt_value bt_value;

typedef const char * const *bt_field_class_enumeration_mapping_label_array;
typedef const struct bt_message **bt_message_array_const;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_TYPES_H */
