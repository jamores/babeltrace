#ifndef BABELTRACE2_TRACE_IR_FIELD_PATH_CONST_H
#define BABELTRACE2_TRACE_IR_FIELD_PATH_CONST_H

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

#include <babeltrace2/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bt_field_path_item_type {
	BT_FIELD_PATH_ITEM_TYPE_INDEX				= 1 << 0,
	BT_FIELD_PATH_ITEM_TYPE_CURRENT_ARRAY_ELEMENT		= 1 << 1,
	BT_FIELD_PATH_ITEM_TYPE_CURRENT_OPTION_CONTENT		= 1 << 2,
} bt_field_path_item_type;

typedef enum bt_field_path_scope {
	BT_FIELD_PATH_SCOPE_PACKET_CONTEXT		= 0,
	BT_FIELD_PATH_SCOPE_EVENT_COMMON_CONTEXT	= 1,
	BT_FIELD_PATH_SCOPE_EVENT_SPECIFIC_CONTEXT	= 2,
	BT_FIELD_PATH_SCOPE_EVENT_PAYLOAD		= 3,
} bt_field_path_scope;

extern bt_field_path_scope bt_field_path_get_root_scope(
		const bt_field_path *field_path);

extern uint64_t bt_field_path_get_item_count(
		const bt_field_path *field_path);

extern const bt_field_path_item *bt_field_path_borrow_item_by_index_const(
		const bt_field_path *field_path, uint64_t index);

extern bt_field_path_item_type bt_field_path_item_get_type(
		const bt_field_path_item *field_path_item);

extern uint64_t bt_field_path_item_index_get_index(
		const bt_field_path_item *field_path_item);

extern void bt_field_path_get_ref(const bt_field_path *field_path);

extern void bt_field_path_put_ref(const bt_field_path *field_path);

#define BT_FIELD_PATH_PUT_REF_AND_RESET(_var)		\
	do {						\
		bt_field_path_put_ref(_var);		\
		(_var) = NULL;				\
	} while (0)

#define BT_FIELD_PATH_MOVE_REF(_var_dst, _var_src)	\
	do {						\
		bt_field_path_put_ref(_var_dst);	\
		(_var_dst) = (_var_src);		\
		(_var_src) = NULL;			\
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_TRACE_IR_FIELD_PATH_CONST_H */
