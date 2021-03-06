#ifndef BABELTRACE2_VALUE_H
#define BABELTRACE2_VALUE_H

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
#include <stddef.h>

#include <babeltrace2/types.h>
#include <babeltrace2/value-const.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bt_value *const bt_value_null;

extern bt_value *bt_value_bool_create(void);

extern bt_value *bt_value_bool_create_init(bt_bool val);

extern void bt_value_bool_set(bt_value *bool_obj, bt_bool val);

extern bt_value *bt_value_integer_unsigned_create(void);

extern bt_value *bt_value_integer_unsigned_create_init(uint64_t val);

extern void bt_value_integer_unsigned_set(bt_value *integer_obj, uint64_t val);

extern bt_value *bt_value_integer_signed_create(void);

extern bt_value *bt_value_integer_signed_create_init(int64_t val);

extern void bt_value_integer_signed_set(bt_value *integer_obj, int64_t val);

extern bt_value *bt_value_real_create(void);

extern bt_value *bt_value_real_create_init(double val);

extern void bt_value_real_set(bt_value *real_obj, double val);

extern bt_value *bt_value_string_create(void);

extern bt_value *bt_value_string_create_init(const char *val);

typedef enum bt_value_string_set_status {
	BT_VALUE_STRING_SET_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_STRING_SET_STATUS_OK		= __BT_FUNC_STATUS_OK,
} bt_value_string_set_status;

extern bt_value_string_set_status bt_value_string_set(bt_value *string_obj,
		const char *val);

extern bt_value *bt_value_array_create(void);

extern bt_value *bt_value_array_borrow_element_by_index(bt_value *array_obj,
		uint64_t index);

typedef enum bt_value_array_append_element_status {
	BT_VALUE_ARRAY_APPEND_ELEMENT_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_ARRAY_APPEND_ELEMENT_STATUS_OK			= __BT_FUNC_STATUS_OK,
} bt_value_array_append_element_status;

extern bt_value_array_append_element_status bt_value_array_append_element(
		bt_value *array_obj, bt_value *element_obj);

extern bt_value_array_append_element_status
bt_value_array_append_bool_element(bt_value *array_obj, bt_bool val);

extern bt_value_array_append_element_status
bt_value_array_append_unsigned_integer_element(bt_value *array_obj,
		uint64_t val);

extern bt_value_array_append_element_status
bt_value_array_append_signed_integer_element(bt_value *array_obj, int64_t val);

extern bt_value_array_append_element_status
bt_value_array_append_real_element(bt_value *array_obj, double val);

extern bt_value_array_append_element_status
bt_value_array_append_string_element(bt_value *array_obj, const char *val);

extern bt_value_array_append_element_status
bt_value_array_append_empty_array_element(bt_value *array_obj,
		bt_value **element_obj);

extern bt_value_array_append_element_status
bt_value_array_append_empty_map_element(bt_value *array_obj,
		bt_value **element_obj);

typedef enum bt_value_array_set_element_by_index_status {
	BT_VALUE_ARRAY_SET_ELEMENT_BY_INDEX_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_ARRAY_SET_ELEMENT_BY_INDEX_STATUS_OK		= __BT_FUNC_STATUS_OK,
} bt_value_array_set_element_by_index_status;

extern bt_value_array_set_element_by_index_status
bt_value_array_set_element_by_index(bt_value *array_obj, uint64_t index,
		bt_value *element_obj);

extern bt_value *bt_value_map_create(void);

extern bt_value *bt_value_map_borrow_entry_value(
		bt_value *map_obj, const char *key);

typedef enum bt_value_map_foreach_entry_func_status {
	BT_VALUE_MAP_FOREACH_ENTRY_FUNC_STATUS_OK		= __BT_FUNC_STATUS_OK,
	BT_VALUE_MAP_FOREACH_ENTRY_FUNC_STATUS_ERROR		= __BT_FUNC_STATUS_ERROR,
	BT_VALUE_MAP_FOREACH_ENTRY_FUNC_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_MAP_FOREACH_ENTRY_FUNC_STATUS_INTERRUPT	= __BT_FUNC_STATUS_INTERRUPTED,
} bt_value_map_foreach_entry_func_status;

typedef bt_value_map_foreach_entry_func_status
		(* bt_value_map_foreach_entry_func)(const char *key,
			bt_value *object, void *data);

typedef enum bt_value_map_foreach_entry_status {
	BT_VALUE_MAP_FOREACH_ENTRY_STATUS_OK		= __BT_FUNC_STATUS_OK,
	BT_VALUE_MAP_FOREACH_ENTRY_STATUS_ERROR		= __BT_FUNC_STATUS_ERROR,
	BT_VALUE_MAP_FOREACH_ENTRY_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_MAP_FOREACH_ENTRY_STATUS_USER_ERROR	= __BT_FUNC_STATUS_USER_ERROR,
	BT_VALUE_MAP_FOREACH_ENTRY_STATUS_INTERRUPTED	= __BT_FUNC_STATUS_INTERRUPTED,
} bt_value_map_foreach_entry_status;

extern bt_value_map_foreach_entry_status bt_value_map_foreach_entry(
		bt_value *map_obj, bt_value_map_foreach_entry_func func,
		void *data);

typedef enum bt_value_map_insert_entry_status {
	BT_VALUE_MAP_INSERT_ENTRY_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_MAP_INSERT_ENTRY_STATUS_OK		= __BT_FUNC_STATUS_OK,
} bt_value_map_insert_entry_status;

extern bt_value_map_insert_entry_status bt_value_map_insert_entry(
		bt_value *map_obj, const char *key, bt_value *element_obj);

extern bt_value_map_insert_entry_status bt_value_map_insert_bool_entry(
		bt_value *map_obj, const char *key, bt_bool val);

extern bt_value_map_insert_entry_status
bt_value_map_insert_unsigned_integer_entry(bt_value *map_obj, const char *key,
		uint64_t val);

extern bt_value_map_insert_entry_status
bt_value_map_insert_signed_integer_entry(bt_value *map_obj, const char *key,
		int64_t val);

extern bt_value_map_insert_entry_status bt_value_map_insert_real_entry(
		bt_value *map_obj, const char *key, double val);

extern bt_value_map_insert_entry_status
bt_value_map_insert_string_entry(bt_value *map_obj, const char *key,
		const char *val);

extern bt_value_map_insert_entry_status
bt_value_map_insert_empty_array_entry(bt_value *map_obj, const char *key,
		bt_value **entry_obj);

extern bt_value_map_insert_entry_status
bt_value_map_insert_empty_map_entry(bt_value *map_obj, const char *key,
		bt_value **entry_obj);

typedef enum bt_value_map_extend_status {
	BT_VALUE_MAP_EXTEND_STATUS_MEMORY_ERROR	= __BT_FUNC_STATUS_MEMORY_ERROR,
	BT_VALUE_MAP_EXTEND_STATUS_OK		= __BT_FUNC_STATUS_OK,
} bt_value_map_extend_status;

extern bt_value_map_extend_status bt_value_map_extend(
		bt_value *base_map_obj,
		const bt_value *extension_map_obj);

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE2_VALUE_H */
