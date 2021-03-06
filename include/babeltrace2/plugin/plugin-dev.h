#ifndef BABELTRACE2_PLUGIN_PLUGIN_DEV_H
#define BABELTRACE2_PLUGIN_PLUGIN_DEV_H

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

#include <babeltrace2/graph/component-class-const.h>
#include <babeltrace2/graph/component-class-source.h>
#include <babeltrace2/graph/component-class-filter.h>
#include <babeltrace2/graph/component-class-sink.h>
#include <babeltrace2/graph/message-iterator-class.h>
#include <babeltrace2/types.h>

/*
 * _BT_HIDDEN: set the hidden attribute for internal functions
 * On Windows, symbols are local unless explicitly exported,
 * see https://gcc.gnu.org/wiki/Visibility
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#define _BT_HIDDEN
#else
#define _BT_HIDDEN __attribute__((visibility("hidden")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Plugin initialization function type */
typedef enum bt_plugin_initialize_func_status {
	BT_PLUGIN_INITIALIZE_FUNC_STATUS_OK		= __BT_FUNC_STATUS_OK,
	BT_PLUGIN_INITIALIZE_FUNC_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_PLUGIN_INITIALIZE_FUNC_STATUS_ERROR		= __BT_FUNC_STATUS_ERROR,
} bt_plugin_initialize_func_status;

typedef bt_plugin_initialize_func_status (*bt_plugin_initialize_func)(
		bt_self_plugin *plugin);

/* Plugin exit function type */
typedef void (*bt_plugin_finalize_func)(void);

/* Plugin descriptor: describes a single plugin (internal use) */
struct __bt_plugin_descriptor {
	/* Plugin's name */
	const char *name;
} __attribute__((packed));

/* Type of a plugin attribute (internal use) */
enum __bt_plugin_descriptor_attribute_type {
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_INIT		= 0,
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_EXIT		= 1,
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_AUTHOR		= 2,
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_LICENSE		= 3,
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION		= 4,
	BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_VERSION		= 5,
};

/* Plugin (user) version */
struct __bt_plugin_descriptor_version {
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
	const char *extra;
};

/* Plugin attribute (internal use) */
struct __bt_plugin_descriptor_attribute {
	/* Plugin descriptor to which to associate this attribute */
	const struct __bt_plugin_descriptor *plugin_descriptor;

	/* Name of the attribute's type for debug purposes */
	const char *type_name;

	/* Attribute's type */
	enum __bt_plugin_descriptor_attribute_type type;

	/* Attribute's value (depends on attribute's type) */
	union {
		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_INIT */
		bt_plugin_initialize_func init;

		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_EXIT */
		bt_plugin_finalize_func exit;

		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_AUTHOR */
		const char *author;

		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_LICENSE */
		const char *license;

		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION */
		const char *description;

		/* BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_VERSION */
		struct __bt_plugin_descriptor_version version;
	} value;
} __attribute__((packed));

/* Component class descriptor (internal use) */
struct __bt_plugin_component_class_descriptor {
	/*
	 * Plugin descriptor to which to associate this component
	 * class descriptor.
	 */
	const struct __bt_plugin_descriptor *plugin_descriptor;

	/* Component class name */
	const char *name;

	/* Component class type */
	bt_component_class_type type;

	/* Mandatory methods (depends on component class type) */
	union {
		/* BT_COMPONENT_CLASS_TYPE_SOURCE */
		struct {
			bt_message_iterator_class_next_method msg_iter_next;
		} source;

		/* BT_COMPONENT_CLASS_TYPE_FILTER */
		struct {
			bt_message_iterator_class_next_method msg_iter_next;
		} filter;

		/* BT_COMPONENT_CLASS_TYPE_SINK */
		struct {
			bt_component_class_sink_consume_method consume;
		} sink;
	} methods;
} __attribute__((packed));

/* Type of a component class attribute (internal use) */
enum __bt_plugin_component_class_descriptor_attribute_type {
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION					= 0,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_HELP					= 1,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GET_SUPPORTED_MIP_VERSIONS_METHOD		= 2,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INITIALIZE_METHOD					= 3,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_FINALIZE_METHOD				= 4,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_QUERY_METHOD				= 5,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INPUT_PORT_CONNECTED_METHOD			= 6,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_OUTPUT_PORT_CONNECTED_METHOD		= 7,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GRAPH_IS_CONFIGURED_METHOD			= 8,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_INITIALIZE_METHOD			= 9,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_FINALIZE_METHOD			= 10,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_NS_FROM_ORIGIN_METHOD		= 11,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_BEGINNING_METHOD		= 12,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_NS_FROM_ORIGIN_METHOD	= 13,
	BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_BEGINNING_METHOD		= 14,
};

/* Component class attribute (internal use) */
struct __bt_plugin_component_class_descriptor_attribute {
	/*
	 * Component class plugin attribute to which to associate this
	 * component class attribute.
	 */
	const struct __bt_plugin_component_class_descriptor *comp_class_descriptor;

	/* Name of the attribute's type for debug purposes */
	const char *type_name;

	/* Attribute's type */
	enum __bt_plugin_component_class_descriptor_attribute_type type;

	/* Attribute's value (depends on attribute's type) */
	union {
		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION */
		const char *description;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_HELP */
		const char *help;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GET_SUPPORTED_MIP_VERSIONS_METHOD */
		bt_component_class_source_get_supported_mip_versions_method source_get_supported_mip_versions_method;
		bt_component_class_filter_get_supported_mip_versions_method filter_get_supported_mip_versions_method;
		bt_component_class_sink_get_supported_mip_versions_method sink_get_supported_mip_versions_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INITIALIZE_METHOD */
		bt_component_class_source_initialize_method source_initialize_method;
		bt_component_class_filter_initialize_method filter_initialize_method;
		bt_component_class_sink_initialize_method sink_initialize_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_FINALIZE_METHOD */
		bt_component_class_source_finalize_method source_finalize_method;
		bt_component_class_filter_finalize_method filter_finalize_method;
		bt_component_class_sink_finalize_method sink_finalize_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_QUERY_METHOD */
		bt_component_class_source_query_method source_query_method;
		bt_component_class_filter_query_method filter_query_method;
		bt_component_class_sink_query_method sink_query_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INPUT_PORT_CONNECTED_METHOD */
		bt_component_class_filter_input_port_connected_method filter_input_port_connected_method;
		bt_component_class_sink_input_port_connected_method sink_input_port_connected_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_OUTPUT_PORT_CONNECTED_METHOD */
		bt_component_class_source_output_port_connected_method source_output_port_connected_method;
		bt_component_class_filter_output_port_connected_method filter_output_port_connected_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GRAPH_IS_CONFIGURED_METHOD */
		bt_component_class_sink_graph_is_configured_method sink_graph_is_configured_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_INITIALIZE_METHOD */
		bt_message_iterator_class_initialize_method msg_iter_initialize_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_FINALIZE_METHOD */
		bt_message_iterator_class_finalize_method msg_iter_finalize_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_NS_FROM_ORIGIN_METHOD */
		bt_message_iterator_class_seek_ns_from_origin_method msg_iter_seek_ns_from_origin_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_BEGINNING_METHOD */
		bt_message_iterator_class_seek_beginning_method msg_iter_seek_beginning_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_NS_FROM_ORIGIN_METHOD */
		bt_message_iterator_class_can_seek_ns_from_origin_method msg_iter_can_seek_ns_from_origin_method;

		/* BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_BEGINNING_METHOD */
		bt_message_iterator_class_can_seek_beginning_method msg_iter_can_seek_beginning_method;
	} value;
} __attribute__((packed));

struct __bt_plugin_descriptor const * const *__bt_get_begin_section_plugin_descriptors(void);
struct __bt_plugin_descriptor const * const *__bt_get_end_section_plugin_descriptors(void);
struct __bt_plugin_descriptor_attribute const * const *__bt_get_begin_section_plugin_descriptor_attributes(void);
struct __bt_plugin_descriptor_attribute const * const *__bt_get_end_section_plugin_descriptor_attributes(void);
struct __bt_plugin_component_class_descriptor const * const *__bt_get_begin_section_component_class_descriptors(void);
struct __bt_plugin_component_class_descriptor const * const *__bt_get_end_section_component_class_descriptors(void);
struct __bt_plugin_component_class_descriptor_attribute const * const *__bt_get_begin_section_component_class_descriptor_attributes(void);
struct __bt_plugin_component_class_descriptor_attribute const * const *__bt_get_end_section_component_class_descriptor_attributes(void);

/*
 * Variable attributes for a plugin descriptor pointer to be added to
 * the plugin descriptor section (internal use).
 */
#ifdef __APPLE__
#define __BT_PLUGIN_DESCRIPTOR_ATTRS \
	__attribute__((section("__DATA,btp_desc"), used))

#define __BT_PLUGIN_DESCRIPTOR_BEGIN_SYMBOL \
	__start___bt_plugin_descriptors

#define __BT_PLUGIN_DESCRIPTOR_END_SYMBOL \
	__stop___bt_plugin_descriptors

#define __BT_PLUGIN_DESCRIPTOR_BEGIN_EXTRA \
	__asm("section$start$__DATA$btp_desc")

#define __BT_PLUGIN_DESCRIPTOR_END_EXTRA \
	__asm("section$end$__DATA$btp_desc")

#else

#define __BT_PLUGIN_DESCRIPTOR_ATTRS \
	__attribute__((section("__bt_plugin_descriptors"), used))

#define __BT_PLUGIN_DESCRIPTOR_BEGIN_SYMBOL \
	__start___bt_plugin_descriptors

#define __BT_PLUGIN_DESCRIPTOR_END_SYMBOL \
	__stop___bt_plugin_descriptors

#define __BT_PLUGIN_DESCRIPTOR_BEGIN_EXTRA

#define __BT_PLUGIN_DESCRIPTOR_END_EXTRA
#endif

/*
 * Variable attributes for a plugin attribute pointer to be added to
 * the plugin attribute section (internal use).
 */
#ifdef __APPLE__
#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_ATTRS \
	__attribute__((section("__DATA,btp_desc_att"), used))

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL \
	__start___bt_plugin_descriptor_attributes

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_SYMBOL \
	__stop___bt_plugin_descriptor_attributes

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA \
	__asm("section$start$__DATA$btp_desc_att")

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_EXTRA \
	__asm("section$end$__DATA$btp_desc_att")

#else

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_ATTRS \
	__attribute__((section("__bt_plugin_descriptor_attributes"), used))

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL \
	__start___bt_plugin_descriptor_attributes

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_SYMBOL \
	__stop___bt_plugin_descriptor_attributes

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA

#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_EXTRA
#endif

/*
 * Variable attributes for a component class descriptor pointer to be
 * added to the component class descriptor section (internal use).
 */
#ifdef __APPLE__
#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS \
	__attribute__((section("__DATA,btp_cc_desc"), used))

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_SYMBOL \
	__start___bt_plugin_component_class_descriptors

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_SYMBOL \
	__stop___bt_plugin_component_class_descriptors

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_EXTRA \
	__asm("section$start$__DATA$btp_cc_desc")

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_EXTRA \
	__asm("section$end$__DATA$btp_cc_desc")

#else

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS \
	__attribute__((section("__bt_plugin_component_class_descriptors"), used))

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_SYMBOL \
	__start___bt_plugin_component_class_descriptors

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_SYMBOL \
	__stop___bt_plugin_component_class_descriptors

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_EXTRA

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_EXTRA
#endif

/*
 * Variable attributes for a component class descriptor attribute
 * pointer to be added to the component class descriptor attribute
 * section (internal use).
 */
#ifdef __APPLE__
#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_ATTRS \
	__attribute__((section("__DATA,btp_cc_desc_att"), used))

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL \
	__start___bt_plugin_component_class_descriptor_attributes

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_SYMBOL \
	__stop___bt_plugin_component_class_descriptor_attributes

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA \
	__asm("section$start$__DATA$btp_cc_desc_att")

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_EXTRA \
	__asm("section$end$__DATA$btp_cc_desc_att")

#else

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_ATTRS \
	__attribute__((section("__bt_plugin_component_class_descriptor_attributes"), used))

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL \
	__start___bt_plugin_component_class_descriptor_attributes

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_SYMBOL \
	__stop___bt_plugin_component_class_descriptor_attributes

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA

#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_EXTRA
#endif

/*
 * Declares a plugin descriptor pointer variable with a custom ID.
 *
 * _id: ID (any valid C identifier except `auto`).
 */
#define BT_PLUGIN_DECLARE(_id) extern struct __bt_plugin_descriptor __bt_plugin_descriptor_##_id

/*
 * Defines a plugin descriptor with a custom ID.
 *
 * _id:   ID (any valid C identifier except `auto`).
 * _name: Plugin's name (C string).
 */
#define BT_PLUGIN_WITH_ID(_id, _name)					\
	struct __bt_plugin_descriptor __bt_plugin_descriptor_##_id = {	\
		.name = _name,						\
	};								\
	static struct __bt_plugin_descriptor const * const __bt_plugin_descriptor_##_id##_ptr __BT_PLUGIN_DESCRIPTOR_ATTRS = &__bt_plugin_descriptor_##_id

/*
 * Defines a plugin attribute (generic, internal use).
 *
 * _attr_name: Name of the attribute (C identifier).
 * _attr_type: Type of the attribute (enum __bt_plugin_descriptor_attribute_type).
 * _id:        Plugin descriptor ID (C identifier).
 * _x:         Value.
 */
#define __BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(_attr_name, _attr_type, _id, _x) \
	static struct __bt_plugin_descriptor_attribute __bt_plugin_descriptor_attribute_##_id##_##_attr_name = { \
		.plugin_descriptor = &__bt_plugin_descriptor_##_id,	\
		.type_name = #_attr_name,				\
		.type = _attr_type,					\
		.value = { ._attr_name = _x },				\
	};								\
	static struct __bt_plugin_descriptor_attribute const * const __bt_plugin_descriptor_attribute_##_id##_##_attr_name##_ptr __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_ATTRS = &__bt_plugin_descriptor_attribute_##_id##_##_attr_name

/*
 * Defines a plugin initialization function attribute attached to a
 * specific plugin descriptor.
 *
 * _id: Plugin descriptor ID (C identifier).
 * _x:  Initialization function (bt_plugin_initialize_func).
 */
#define BT_PLUGIN_INITIALIZE_FUNC_WITH_ID(_id, _x) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(init, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_INIT, _id, _x)

/*
 * Defines a plugin exit function attribute attached to a specific
 * plugin descriptor.
 *
 * _id: Plugin descriptor ID (C identifier).
 * _x:  Exit function (bt_plugin_finalize_func).
 */
#define BT_PLUGIN_FINALIZE_FUNC_WITH_ID(_id, _x) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(exit, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_EXIT, _id, _x)

/*
 * Defines an author attribute attached to a specific plugin descriptor.
 *
 * _id: Plugin descriptor ID (C identifier).
 * _x:  Author (C string).
 */
#define BT_PLUGIN_AUTHOR_WITH_ID(_id, _x) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(author, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_AUTHOR, _id, _x)

/*
 * Defines a license attribute attached to a specific plugin descriptor.
 *
 * _id: Plugin descriptor ID (C identifier).
 * _x:  License (C string).
 */
#define BT_PLUGIN_LICENSE_WITH_ID(_id, _x) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(license, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_LICENSE, _id, _x)

/*
 * Defines a description attribute attached to a specific plugin
 * descriptor.
 *
 * _id: Plugin descriptor ID (C identifier).
 * _x:  Description (C string).
 */
#define BT_PLUGIN_DESCRIPTION_WITH_ID(_id, _x) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(description, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION, _id, _x)

#define __BT_PLUGIN_VERSION_STRUCT_VALUE(_major, _minor, _patch, _extra) \
	{.major = _major, .minor = _minor, .patch = _patch, .extra = _extra,}

/*
 * Defines a version attribute attached to a specific plugin descriptor.
 *
 * _id:    Plugin descriptor ID (C identifier).
 * _major: Plugin's major version (uint32_t).
 * _minor: Plugin's minor version (uint32_t).
 * _patch: Plugin's patch version (uint32_t).
 * _extra: Plugin's version extra information (C string).
 */
#define BT_PLUGIN_VERSION_WITH_ID(_id, _major, _minor, _patch, _extra) \
	__BT_PLUGIN_DESCRIPTOR_ATTRIBUTE(version, BT_PLUGIN_DESCRIPTOR_ATTRIBUTE_TYPE_VERSION, _id, __BT_PLUGIN_VERSION_STRUCT_VALUE(_major, _minor, _patch, _extra))

/*
 * Defines a source component class descriptor with a custom ID.
 *
 * _id:                     ID (any valid C identifier except `auto`).
 * _comp_class_id:          Component class ID (C identifier).
 * _name:                   Component class name (C string).
 * _msg_iter_next_method: Component class's iterator next method
 *                          (bt_component_class_source_message_iterator_next_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_WITH_ID(_id, _comp_class_id, _name, _msg_iter_next_method) \
	static struct __bt_plugin_component_class_descriptor __bt_plugin_source_component_class_descriptor_##_id##_##_comp_class_id = { \
		.plugin_descriptor = &__bt_plugin_descriptor_##_id,			\
		.name = _name,								\
		.type = BT_COMPONENT_CLASS_TYPE_SOURCE,					\
		.methods = { .source = { .msg_iter_next = _msg_iter_next_method } },	\
	};										\
	static struct __bt_plugin_component_class_descriptor const * const __bt_plugin_source_component_class_descriptor_##_id##_##_comp_class_id##_ptr __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS = &__bt_plugin_source_component_class_descriptor_##_id##_##_comp_class_id

/*
 * Defines a filter component class descriptor with a custom ID.
 *
 * _id:                     ID (any valid C identifier except `auto`).
 * _comp_class_id:          Component class ID (C identifier).
 * _name:                   Component class name (C string).
 * _msg_iter_next_method: Component class's iterator next method
 *                          (bt_component_class_filter_message_iterator_next_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_WITH_ID(_id, _comp_class_id, _name, _msg_iter_next_method) \
	static struct __bt_plugin_component_class_descriptor __bt_plugin_filter_component_class_descriptor_##_id##_##_comp_class_id = { \
		.plugin_descriptor = &__bt_plugin_descriptor_##_id,			\
		.name = _name,								\
		.type = BT_COMPONENT_CLASS_TYPE_FILTER,					\
		.methods = { .filter = { .msg_iter_next = _msg_iter_next_method } },	\
	};										\
	static struct __bt_plugin_component_class_descriptor const * const __bt_plugin_filter_component_class_descriptor_##_id##_##_comp_class_id##_ptr __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS = &__bt_plugin_filter_component_class_descriptor_##_id##_##_comp_class_id

/*
 * Defines a sink component class descriptor with a custom ID.
 *
 * _id:                 ID (any valid C identifier except `auto`).
 * _comp_class_id:      Component class ID (C identifier).
 * _name:               Component class name (C string).
 * _consume_method:     Component class's iterator consume method
 *                      (bt_component_class_sink_consume_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_WITH_ID(_id, _comp_class_id, _name, _consume_method) \
	static struct __bt_plugin_component_class_descriptor __bt_plugin_sink_component_class_descriptor_##_id##_##_comp_class_id = { \
		.plugin_descriptor = &__bt_plugin_descriptor_##_id,	\
		.name = _name,						\
		.type = BT_COMPONENT_CLASS_TYPE_SINK,			\
		.methods = { .sink = { .consume = _consume_method } },	\
	};								\
	static struct __bt_plugin_component_class_descriptor const * const __bt_plugin_sink_component_class_descriptor_##_id##_##_comp_class_id##_ptr __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS = &__bt_plugin_sink_component_class_descriptor_##_id##_##_comp_class_id

/*
 * Defines a component class descriptor attribute (generic, internal
 * use).
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class ID (C identifier).
 * _type:          Component class type (`source`, `filter`, or `sink`).
 * _attr_name:     Name of the attribute (C identifier).
 * _attr_type:     Type of the attribute
 *                 (enum __bt_plugin_descriptor_attribute_type).
 * _x:             Value.
 */
#define __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(_attr_name, _attr_type, _id, _comp_class_id, _type, _x) \
	static struct __bt_plugin_component_class_descriptor_attribute __bt_plugin_##_type##_component_class_descriptor_attribute_##_id##_##_comp_class_id##_##_attr_name = { \
		.comp_class_descriptor = &__bt_plugin_##_type##_component_class_descriptor_##_id##_##_comp_class_id, \
		.type_name = #_attr_name,				\
		.type = _attr_type,					\
		.value = { ._attr_name = _x },				\
	};								\
	static struct __bt_plugin_component_class_descriptor_attribute const * const __bt_plugin_##_type##_component_class_descriptor_attribute_##_id##_##_comp_class_id##_##_attr_name##_ptr __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_ATTRS = &__bt_plugin_##_type##_component_class_descriptor_attribute_##_id##_##_comp_class_id##_##_attr_name

/*
 * Defines a description attribute attached to a specific source
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Description (C string).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_DESCRIPTION_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(description, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION, _id, _comp_class_id, source, _x)

/*
 * Defines a description attribute attached to a specific filter
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Description (C string).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_DESCRIPTION_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(description, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION, _id, _comp_class_id, filter, _x)

/*
 * Defines a description attribute attached to a specific sink
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Description (C string).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_DESCRIPTION_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(description, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_DESCRIPTION, _id, _comp_class_id, sink, _x)

/*
 * Defines a help attribute attached to a specific source component
 * class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Help (C string).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_HELP_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(help, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_HELP, _id, _comp_class_id, source, _x)

/*
 * Defines a help attribute attached to a specific filter component
 * class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Help (C string).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_HELP_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(help, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_HELP, _id, _comp_class_id, filter, _x)

/*
 * Defines a help attribute attached to a specific sink component class
 * descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Help (C string).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_HELP_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(help, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_HELP, _id, _comp_class_id, sink, _x)

/*
 * Defines an initialization method attribute attached to a specific
 * source component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Initialization method (bt_component_class_source_initialize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(source_initialize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INITIALIZE_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines an initialization method attribute attached to a specific
 * filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Initialization method (bt_component_class_filter_initialize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_initialize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INITIALIZE_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines an initialization method attribute attached to a specific
 * sink component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Initialization method (bt_component_class_sink_initialize_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_initialize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INITIALIZE_METHOD, _id, _comp_class_id, sink, _x)

/*
 * Defines a "get supported MIP versions" attribute attached to a
 * specific source component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             "Get supported MIP versions" method (bt_component_class_source_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(source_get_supported_mip_versions_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GET_SUPPORTED_MIP_VERSIONS, _id, _comp_class_id, source, _x)

/*
 * Defines a "get supported MIP versions" attribute attached to a
 * specific filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             "Get supported MIP versions" method (bt_component_class_filter_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_get_supported_mip_versions_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GET_SUPPORTED_MIP_VERSIONS, _id, _comp_class_id, filter, _x)

/*
 * Defines a "get supported MIP versions" attribute attached to a
 * specific sink component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             "Get supported MIP versions" method (bt_component_class_sink_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_get_supported_mip_versions_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GET_SUPPORTED_MIP_VERSIONS, _id, _comp_class_id, sink, _x)

/*
 * Defines a finalization method attribute attached to a specific source
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_source_finalize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(source_finalize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_FINALIZE_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines a finalization method attribute attached to a specific filter
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_filter_finalize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_finalize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_FINALIZE_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines a finalization method attribute attached to a specific sink
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_sink_finalize_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_finalize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_FINALIZE_METHOD, _id, _comp_class_id, sink, _x)

/*
 * Defines a query method attribute attached to a specific source
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_source_query_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(source_query_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_QUERY_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines a query method attribute attached to a specific filter
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_filter_query_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_query_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_QUERY_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines a query method attribute attached to a specific sink
 * component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Finalize method (bt_component_class_sink_query_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_query_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_QUERY_METHOD, _id, _comp_class_id, sink, _x)

/*
 * Defines an input port connected method attribute attached to a
 * specific filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Port connected method
 *                 (bt_component_class_filter_input_port_connected_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_input_port_connected_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INPUT_PORT_CONNECTED_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines an input port connected method attribute attached to a
 * specific sink component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Port connected method
 *                 (bt_component_class_sink_input_port_connected_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_input_port_connected_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_INPUT_PORT_CONNECTED_METHOD, _id, _comp_class_id, sink, _x)

/*
 * Defines an output port connected method attribute attached to a
 * specific source component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Port connected method
 *                 (bt_component_class_source_output_port_connected_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(source_output_port_connected_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_OUTPUT_PORT_CONNECTED_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines an output port connected method attribute attached to a
 * specific filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Port connected method
 *                 (bt_component_class_filter_output_port_connected_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(filter_output_port_connected_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_OUTPUT_PORT_CONNECTED_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines a "graph is configured" method attribute attached to a
 * specific sink component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             "Graph is configured" method
 *                 (bt_component_class_sink_graph_is_configured_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_GRAPH_IS_CONFIGURED_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(sink_graph_is_configured_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_GRAPH_IS_CONFIGURED_METHOD, _id, _comp_class_id, sink, _x)

/*
 * Defines an iterator initialization method attribute attached to a
 * specific source component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Iterator initialization method
 *                 (bt_component_class_source_message_iterator_initialize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_initialize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_INITIALIZE_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines an iterator finalize method attribute attached to a specific
 * source component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Iterator finalize method
 *                 (bt_component_class_source_message_iterator_finalize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_finalize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_FINALIZE_METHOD, _id, _comp_class_id, source, _x)

/*
 * Defines an iterator "seek nanoseconds from origin" and "can seek nanoseconds
 * from origin" method attributes attached to a specific source component class
 * descriptor.
 *
 * _id:              Plugin descriptor ID (C identifier).
 * _comp_class_id:   Component class descriptor ID (C identifier).
 * _seek_method:     Iterator "seek nanoseconds from origin" method
 *                   (bt_component_class_source_message_iterator_seek_ns_from_origin_method).
 * _can_seek_method: Iterator "can seek nanoseconds from origin" method
 *                   (bt_component_class_source_message_iterator_can_seek_ns_from_origin_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS_WITH_ID(_id, _comp_class_id, _seek_method, _can_seek_method) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_seek_ns_from_origin_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_NS_FROM_ORIGIN_METHOD, _id, _comp_class_id, source, _seek_method); \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_can_seek_ns_from_origin_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_NS_FROM_ORIGIN_METHOD, _id, _comp_class_id, source, _can_seek_method)

/*
 * Defines an iterator "seek beginning" and "can seek beginning" method
 * attributes attached to a specific source component class descriptor.
 *
 * _id:              Plugin descriptor ID (C identifier).
 * _comp_class_id:   Component class descriptor ID (C identifier).
 * _seek_method:     Iterator "seek beginning" method
 *                   (bt_component_class_source_message_iterator_seek_beginning_method).
 * _can_seek_method: Iterator "can seek beginning" method
 *                   (bt_component_class_source_message_iterator_can_seek_beginning_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS_WITH_ID(_id, _comp_class_id, _seek_method, _can_seek_method) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_seek_beginning_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_BEGINNING_METHOD, _id, _comp_class_id, source, _seek_method); \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_can_seek_beginning_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_BEGINNING_METHOD, _id, _comp_class_id, source, _can_seek_method)

/*
 * Defines an iterator initialization method attribute attached to a
 * specific filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Iterator initialization method
 *                 (bt_component_class_filter_message_iterator_initialize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_initialize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_INITIALIZE_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines an iterator finalize method attribute attached to a specific
 * filter component class descriptor.
 *
 * _id:            Plugin descriptor ID (C identifier).
 * _comp_class_id: Component class descriptor ID (C identifier).
 * _x:             Iterator finalize method
 *                 (bt_component_class_filter_message_iterator_finalize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD_WITH_ID(_id, _comp_class_id, _x) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_finalize_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_FINALIZE_METHOD, _id, _comp_class_id, filter, _x)

/*
 * Defines an iterator "seek nanoseconds" and "can seek nanoseconds from origin"
 * method attributes attached to a specific filter component class descriptor.
 *
 * _id:              Plugin descriptor ID (C identifier).
 * _comp_class_id:   Component class descriptor ID (C identifier).
 * _seek_method:     Iterator "seek nanoseconds from origin" method
 *                   (bt_component_class_filter_message_iterator_seek_ns_from_origin_method).
 * _can_seek_method: Iterator "can seek nanoseconds from origin" method
 *                   (bt_component_class_filter_message_iterator_can_seek_ns_from_origin_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS_WITH_ID(_id, _comp_class_id, _seek_method, _can_seek_method) \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_seek_ns_from_origin_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_NS_FROM_ORIGIN_METHOD, _id, _comp_class_id, filter, _seek_method); \
	__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_can_seek_ns_from_origin_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_NS_FROM_ORIGIN_METHOD, _id, _comp_class_id, filter, _can_seek_method)

/*
 * Defines an iterator "seek beginning" and "can seek beginning" method
 * attributes attached to a specific filter component class descriptor.
 *
 * _id:              Plugin descriptor ID (C identifier).
 * _comp_class_id:   Component class descriptor ID (C identifier).
 * _seek_method:     Iterator "seek beginning" method
 *                   (bt_component_class_filter_message_iterator_seek_beginning_method).
 * _can_seek_method: Iterator "can seek beginning" method
 *                   (bt_component_class_filter_message_iterator_can_seek_beginning_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS_WITH_ID(_id, _comp_class_id, _seek_method, _can_seek_method) \
		__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_seek_beginning_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_SEEK_BEGINNING_METHOD, _id, _comp_class_id, filter, _seek_method); \
		__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE(msg_iter_can_seek_beginning_method, BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTE_TYPE_MSG_ITER_CAN_SEEK_BEGINNING_METHOD, _id, _comp_class_id, filter, _can_seek_method);

/*
 * Defines a plugin descriptor with an automatic ID.
 *
 * _name: Plugin's name (C string).
 */
#define BT_PLUGIN(_name) 		static BT_PLUGIN_WITH_ID(auto, #_name)

/*
 * Defines a plugin initialization function attribute attached to the
 * automatic plugin descriptor.
 *
 * _x: Initialization function (bt_plugin_initialize_func).
 */
#define BT_PLUGIN_INITIALIZE_FUNC(_x) 	BT_PLUGIN_INITIALIZE_FUNC_WITH_ID(auto, _x)

 /*
 * Defines a plugin exit function attribute attached to the automatic
 * plugin descriptor.
 *
 * _x: Exit function (bt_plugin_finalize_func).
 */
#define BT_PLUGIN_FINALIZE_FUNC(_x)	BT_PLUGIN_FINALIZE_FUNC_WITH_ID(auto, _x)

/*
 * Defines an author attribute attached to the automatic plugin
 * descriptor.
 *
 * _x: Author (C string).
 */
#define BT_PLUGIN_AUTHOR(_x) 		BT_PLUGIN_AUTHOR_WITH_ID(auto, _x)

/*
 * Defines a license attribute attached to the automatic plugin
 * descriptor.
 *
 * _x: License (C string).
 */
#define BT_PLUGIN_LICENSE(_x) 		BT_PLUGIN_LICENSE_WITH_ID(auto, _x)

/*
 * Defines a description attribute attached to the automatic plugin
 * descriptor.
 *
 * _x: Description (C string).
 */
#define BT_PLUGIN_DESCRIPTION(_x) 	BT_PLUGIN_DESCRIPTION_WITH_ID(auto, _x)

/*
 * Defines a version attribute attached to the automatic plugin
 * descriptor.
 *
 * _major: Plugin's major version (uint32_t).
 * _minor: Plugin's minor version (uint32_t).
 * _patch: Plugin's patch version (uint32_t).
 * _extra: Plugin's version extra information (C string).
 */
#define BT_PLUGIN_VERSION(_major, _minor, _patch, _extra) BT_PLUGIN_VERSION_WITH_ID(auto, _major, _minor, _patch, _extra)

/*
 * Defines a source component class attached to the automatic plugin
 * descriptor. Its ID is the same as its name, hence its name must be a
 * C identifier in this version.
 *
 * _name:                   Component class name (C identifier).
 * _msg_iter_next_method: Component class's iterator next method
 *                          (bt_component_class_source_message_iterator_next_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS(_name, _msg_iter_next_method) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_WITH_ID(auto, _name, #_name, _msg_iter_next_method)

/*
 * Defines a filter component class attached to the automatic plugin
 * descriptor. Its ID is the same as its name, hence its name must be a
 * C identifier in this version.
 *
 * _name:                   Component class name (C identifier).
 * _msg_iter_next_method: Component class's iterator next method
 *                          (bt_component_class_filter_message_iterator_next_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS(_name, _msg_iter_next_method) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_WITH_ID(auto, _name, #_name, _msg_iter_next_method)

/*
 * Defines a sink component class attached to the automatic plugin
 * descriptor. Its ID is the same as its name, hence its name must be a
 * C identifier in this version.
 *
 * _name:           Component class name (C identifier).
 * _consume_method: Component class's consume method
 *                  (bt_component_class_sink_consume_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS(_name, _consume_method) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_WITH_ID(auto, _name, #_name, _consume_method)

/*
 * Defines a description attribute attached to a source component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Description (C string).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_DESCRIPTION(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_DESCRIPTION_WITH_ID(auto, _name, _x)

/*
 * Defines a description attribute attached to a filter component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Description (C string).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_DESCRIPTION(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_DESCRIPTION_WITH_ID(auto, _name, _x)

/*
 * Defines a description attribute attached to a sink component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Description (C string).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_DESCRIPTION(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_DESCRIPTION_WITH_ID(auto, _name, _x)

/*
 * Defines a help attribute attached to a source component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Help (C string).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_HELP(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_HELP_WITH_ID(auto, _name, _x)

/*
 * Defines a help attribute attached to a filter component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Help (C string).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_HELP(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_HELP_WITH_ID(auto, _name, _x)

/*
 * Defines a help attribute attached to a sink component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Help (C string).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_HELP(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_HELP_WITH_ID(auto, _name, _x)

/*
 * Defines an initialization method attribute attached to a source
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_source_initialize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_INITIALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an initialization method attribute attached to a filter
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_filter_initialize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_INITIALIZE_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an initialization method attribute attached to a sink
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_sink_initialize_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_INITIALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_INITIALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a "get supported MIP versions" method attribute attached to a
 * source component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_source_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a "get supported MIP versions" method attribute attached to a
 * filter component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_filter_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a "get supported MIP versions" method attribute attached to a
 * sink component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_sink_get_supported_mip_versions_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_GET_SUPPORTED_MIP_VERSIONS_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a finalization method attribute attached to a source component
 * class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_source_finalize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_FINALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a finalization method attribute attached to a filter component
 * class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_filter_finalize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_FINALIZE_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a finalization method attribute attached to a sink component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_sink_finalize_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_FINALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_FINALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a query method attribute attached to a source component
 * class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_source_query_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_QUERY_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a query method attribute attached to a filter component
 * class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_filter_query_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_QUERY_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a query method attribute attached to a sink component
 * class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Initialization method (bt_component_class_sink_query_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_QUERY_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_QUERY_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an input port connected method attribute attached to a filter
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Port connected (bt_component_class_filter_input_port_connected_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an input port connected method attribute attached to a sink
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Port connected (bt_component_class_sink_input_port_connected_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_INPUT_PORT_CONNECTED_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an output port connected method attribute attached to a source
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Port connected (bt_component_class_source_output_port_connected_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an output port connected method attribute attached to a filter
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Port connected (bt_component_class_filter_output_port_connected_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_OUTPUT_PORT_CONNECTED_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines a "graph is configured" method attribute attached to
 * a sink component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    "Graph is configured" method
 *        (bt_component_class_sink_graph_is_configured_method).
 */
#define BT_PLUGIN_SINK_COMPONENT_CLASS_GRAPH_IS_CONFIGURED_METHOD(_name, _x) \
	BT_PLUGIN_SINK_COMPONENT_CLASS_GRAPH_IS_CONFIGURED_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an iterator initialization method attribute attached to a
 * source component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Iterator initialization method
 *        (bt_component_class_source_message_iterator_initialize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an iterator finalize method attribute attached to a source
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Iterator finalize method
 *        (bt_component_class_source_message_iterator_finalize_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD(_name, _x) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an iterator "seek nanoseconds from origin" and "can seek nanoseconds
 * from origin" method attributes attached to a source component class
 * descriptor which is attached to the automatic plugin descriptor.
 *
 * _name:            Component class name (C identifier).
 * _seek_method:     Iterator "seek nanoseconds from origin" method
 *                   (bt_component_class_source_message_iterator_seek_ns_from_origin_method).
 * _can_seek_method: Iterator "can seek nanoseconds from origin" method
 *                   (bt_component_class_source_message_iterator_can_seek_ns_from_origin_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS(_name, _seek_method, _can_seek_method) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS_WITH_ID(auto, _name, _seek_method, _can_seek_method)

/*
 * Defines an iterator "seek beginning" and "can seek beginning" method
 * attributes attached to a source component class descriptor which is attached
 * to the automatic plugin descriptor.
 *
 * _name:            Component class name (C identifier).
 * _seek_method:     Iterator "can seek beginning" method
 *                   (bt_component_class_source_message_iterator_can_seek_beginning_method).
 * _can_seek_method: Iterator "can seek beginning" method
 *                   (bt_component_class_source_message_iterator_seek_beginning_method).
 */
#define BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS(_name, _seek_method, _can_seek_method) \
	BT_PLUGIN_SOURCE_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS_WITH_ID(auto, _name, _seek_method, _can_seek_method)

/*
 * Defines an iterator initialization method attribute attached to a
 * filter component class descriptor which is attached to the automatic
 * plugin descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Iterator initialization method
 *        (bt_component_class_filter_message_iterator_initialize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_INITIALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an iterator finalize method attribute attached to a filter
 * component class descriptor which is attached to the automatic plugin
 * descriptor.
 *
 * _name: Component class name (C identifier).
 * _x:    Iterator finalize method
 *        (bt_component_class_filter_message_iterator_finalize_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD(_name, _x) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_FINALIZE_METHOD_WITH_ID(auto, _name, _x)

/*
 * Defines an iterator "seek nanosecconds from origin" and "can seek
 * nanoseconds from origin" method attributes attached to a filter component
 * class descriptor which is attached to the automatic plugin descriptor.
 *
 * _name:            Component class name (C identifier).
 * _seek_method:     Iterator "seek nanoseconds from origin" method
 *                   (bt_component_class_filter_message_iterator_seek_ns_from_origin_method).
 * _can_seek_method: Iterator "can seek nanoseconds from origin" method
 *                   (bt_component_class_filter_message_iterator_can_seek_ns_from_origin_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS(_name, _seek_method, _can_seek_method) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_NS_FROM_ORIGIN_METHODS_WITH_ID(auto, _name, _seek_method, _can_seek_method)

/*
 * Defines an iterator "seek beginning" and "can seek beginning" method
 * attributes attached to a filter component class descriptor which is attached
 * to the automatic plugin descriptor.
 *
 * _name:            Component class name (C identifier).
 * _seek_method:     Iterator "seek beginning" method
 *                   (bt_component_class_filter_message_iterator_seek_beginning_method).
 * _can_seek_method: Iterator "can seek beginning" method
 *                   (bt_component_class_filter_message_iterator_can_seek_beginning_method).
 */
#define BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS(_name, _seek_method, _can_seek_method) \
	BT_PLUGIN_FILTER_COMPONENT_CLASS_MESSAGE_ITERATOR_CLASS_SEEK_BEGINNING_METHODS_WITH_ID(auto, _name, _seek_method, _can_seek_method)

#define BT_PLUGIN_MODULE() \
	static struct __bt_plugin_descriptor const * const __bt_plugin_descriptor_dummy __BT_PLUGIN_DESCRIPTOR_ATTRS = NULL; \
	_BT_HIDDEN extern struct __bt_plugin_descriptor const *__BT_PLUGIN_DESCRIPTOR_BEGIN_SYMBOL __BT_PLUGIN_DESCRIPTOR_BEGIN_EXTRA; \
	_BT_HIDDEN extern struct __bt_plugin_descriptor const *__BT_PLUGIN_DESCRIPTOR_END_SYMBOL __BT_PLUGIN_DESCRIPTOR_END_EXTRA; \
	\
	static struct __bt_plugin_descriptor_attribute const * const __bt_plugin_descriptor_attribute_dummy __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_ATTRS = NULL; \
	_BT_HIDDEN extern struct __bt_plugin_descriptor_attribute const *__BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA; \
	_BT_HIDDEN extern struct __bt_plugin_descriptor_attribute const *__BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_SYMBOL __BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_EXTRA; \
	\
	static struct __bt_plugin_component_class_descriptor const * const __bt_plugin_component_class_descriptor_dummy __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRS = NULL; \
	_BT_HIDDEN extern struct __bt_plugin_component_class_descriptor const *__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_SYMBOL __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_EXTRA; \
	_BT_HIDDEN extern struct __bt_plugin_component_class_descriptor const *__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_SYMBOL __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_EXTRA; \
	\
	static struct __bt_plugin_component_class_descriptor_attribute const * const __bt_plugin_component_class_descriptor_attribute_dummy __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_ATTRS = NULL; \
	_BT_HIDDEN extern struct __bt_plugin_component_class_descriptor_attribute const *__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_EXTRA; \
	_BT_HIDDEN extern struct __bt_plugin_component_class_descriptor_attribute const *__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_SYMBOL __BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_EXTRA; \
	\
	struct __bt_plugin_descriptor const * const *__bt_get_begin_section_plugin_descriptors(void) \
	{ \
		return &__BT_PLUGIN_DESCRIPTOR_BEGIN_SYMBOL; \
	} \
	struct __bt_plugin_descriptor const * const *__bt_get_end_section_plugin_descriptors(void) \
	{ \
		return &__BT_PLUGIN_DESCRIPTOR_END_SYMBOL; \
	} \
	struct __bt_plugin_descriptor_attribute const * const *__bt_get_begin_section_plugin_descriptor_attributes(void) \
	{ \
		return &__BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL; \
	} \
	struct __bt_plugin_descriptor_attribute const * const *__bt_get_end_section_plugin_descriptor_attributes(void) \
	{ \
		return &__BT_PLUGIN_DESCRIPTOR_ATTRIBUTES_END_SYMBOL; \
	} \
	struct __bt_plugin_component_class_descriptor const * const *__bt_get_begin_section_component_class_descriptors(void) \
	{ \
		return &__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_BEGIN_SYMBOL; \
	} \
	struct __bt_plugin_component_class_descriptor const * const *__bt_get_end_section_component_class_descriptors(void) \
	{ \
		return &__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_END_SYMBOL; \
	} \
	struct __bt_plugin_component_class_descriptor_attribute const * const *__bt_get_begin_section_component_class_descriptor_attributes(void) \
	{ \
		return &__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_BEGIN_SYMBOL; \
	} \
	struct __bt_plugin_component_class_descriptor_attribute const * const *__bt_get_end_section_component_class_descriptor_attributes(void) \
	{ \
		return &__BT_PLUGIN_COMPONENT_CLASS_DESCRIPTOR_ATTRIBUTES_END_SYMBOL; \
	}

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_PLUGIN_PLUGIN_DEV_H */
