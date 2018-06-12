/*
 * fields.c
 *
 * Babeltrace CTF IR - Event Fields
 *
 * Copyright 2013, 2014 Jérémie Galarneau <jeremie.galarneau@efficios.com>
 *
 * Author: Jérémie Galarneau <jeremie.galarneau@efficios.com>
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

#define BT_LOG_TAG "FIELDS"
#include <babeltrace/lib-logging-internal.h>

#include <babeltrace/assert-pre-internal.h>
#include <babeltrace/ctf-ir/fields-internal.h>
#include <babeltrace/ctf-ir/field-types-internal.h>
#include <babeltrace/object-internal.h>
#include <babeltrace/ref.h>
#include <babeltrace/compiler-internal.h>
#include <babeltrace/compat/fcntl-internal.h>
#include <babeltrace/align-internal.h>
#include <babeltrace/assert-internal.h>
#include <inttypes.h>

#define BT_ASSERT_PRE_FIELD_IS_INT_OR_ENUM(_field, _name)		\
	BT_ASSERT_PRE((_field)->type->id == BT_FIELD_TYPE_ID_INTEGER ||	\
		(_field)->type->id == BT_FIELD_TYPE_ID_ENUM,		\
		_name " is not an integer or an enumeration field: "	\
		"%!+f",	(_field))

static struct bt_field_common_methods bt_field_integer_methods = {
	.set_is_frozen = bt_field_common_generic_set_is_frozen,
	.validate = bt_field_common_generic_validate,
	.copy = NULL,
	.is_set = bt_field_common_generic_is_set,
	.reset = bt_field_common_generic_reset,
};

static struct bt_field_common_methods bt_field_floating_point_methods = {
	.set_is_frozen = bt_field_common_generic_set_is_frozen,
	.validate = bt_field_common_generic_validate,
	.copy = NULL,
	.is_set = bt_field_common_generic_is_set,
	.reset = bt_field_common_generic_reset,
};

static struct bt_field_common_methods bt_field_enumeration_methods = {
	.set_is_frozen = bt_field_common_generic_set_is_frozen,
	.validate = bt_field_common_generic_validate,
	.copy = NULL,
	.is_set = bt_field_common_generic_is_set,
	.reset = bt_field_common_generic_reset,
};

static struct bt_field_common_methods bt_field_string_methods = {
	.set_is_frozen = bt_field_common_generic_set_is_frozen,
	.validate = bt_field_common_generic_validate,
	.copy = NULL,
	.is_set = bt_field_common_generic_is_set,
	.reset = bt_field_common_generic_reset,
};

static struct bt_field_common_methods bt_field_structure_methods = {
	.set_is_frozen = bt_field_common_structure_set_is_frozen_recursive,
	.validate = bt_field_common_structure_validate_recursive,
	.copy = NULL,
	.is_set = bt_field_common_structure_is_set_recursive,
	.reset = bt_field_common_structure_reset_recursive,
};

static struct bt_field_common_methods bt_field_sequence_methods = {
	.set_is_frozen = bt_field_common_sequence_set_is_frozen_recursive,
	.validate = bt_field_common_sequence_validate_recursive,
	.copy = NULL,
	.is_set = bt_field_common_sequence_is_set_recursive,
	.reset = bt_field_common_sequence_reset_recursive,
};

static struct bt_field_common_methods bt_field_array_methods = {
	.set_is_frozen = bt_field_common_array_set_is_frozen_recursive,
	.validate = bt_field_common_array_validate_recursive,
	.copy = NULL,
	.is_set = bt_field_common_array_is_set_recursive,
	.reset = bt_field_common_array_reset_recursive,
};

static struct bt_field_common_methods bt_field_variant_methods = {
	.set_is_frozen = bt_field_common_variant_set_is_frozen_recursive,
	.validate = bt_field_common_variant_validate_recursive,
	.copy = NULL,
	.is_set = bt_field_common_variant_is_set_recursive,
	.reset = bt_field_common_variant_reset_recursive,
};

static
struct bt_field *bt_field_integer_create(struct bt_field_type *);

static
struct bt_field *bt_field_enumeration_create(struct bt_field_type *);

static
struct bt_field *bt_field_floating_point_create(struct bt_field_type *);

static
struct bt_field *bt_field_structure_create(struct bt_field_type *);

static
struct bt_field *bt_field_variant_create(struct bt_field_type *);

static
struct bt_field *bt_field_array_create(struct bt_field_type *);

static
struct bt_field *bt_field_sequence_create(struct bt_field_type *);

static
struct bt_field *bt_field_string_create(struct bt_field_type *);

static
struct bt_field *(* const field_create_funcs[])(struct bt_field_type *) = {
	[BT_FIELD_TYPE_ID_INTEGER] =	bt_field_integer_create,
	[BT_FIELD_TYPE_ID_ENUM] =	bt_field_enumeration_create,
	[BT_FIELD_TYPE_ID_FLOAT] =	bt_field_floating_point_create,
	[BT_FIELD_TYPE_ID_STRUCT] =	bt_field_structure_create,
	[BT_FIELD_TYPE_ID_VARIANT] =	bt_field_variant_create,
	[BT_FIELD_TYPE_ID_ARRAY] =	bt_field_array_create,
	[BT_FIELD_TYPE_ID_SEQUENCE] =	bt_field_sequence_create,
	[BT_FIELD_TYPE_ID_STRING] =	bt_field_string_create,
};

static
void bt_field_integer_destroy(struct bt_field *field);

static
void bt_field_enumeration_destroy(struct bt_field *field);

static
void bt_field_floating_point_destroy(struct bt_field *field);

static
void bt_field_structure_destroy_recursive(struct bt_field *field);

static
void bt_field_variant_destroy_recursive(struct bt_field *field);

static
void bt_field_array_destroy_recursive(struct bt_field *field);

static
void bt_field_sequence_destroy_recursive(struct bt_field *field);

static
void bt_field_string_destroy(struct bt_field *field);

static
void (* const field_destroy_funcs[])(struct bt_field *) = {
	[BT_FIELD_TYPE_ID_INTEGER] =	bt_field_integer_destroy,
	[BT_FIELD_TYPE_ID_ENUM] =	bt_field_enumeration_destroy,
	[BT_FIELD_TYPE_ID_FLOAT] =	bt_field_floating_point_destroy,
	[BT_FIELD_TYPE_ID_STRUCT] =	bt_field_structure_destroy_recursive,
	[BT_FIELD_TYPE_ID_VARIANT] =	bt_field_variant_destroy_recursive,
	[BT_FIELD_TYPE_ID_ARRAY] =	bt_field_array_destroy_recursive,
	[BT_FIELD_TYPE_ID_SEQUENCE] =	bt_field_sequence_destroy_recursive,
	[BT_FIELD_TYPE_ID_STRING] =	bt_field_string_destroy,
};

BT_HIDDEN
struct bt_field *bt_field_create_recursive(struct bt_field_type *type)
{
	struct bt_field *field = NULL;
	enum bt_field_type_id type_id;

	BT_ASSERT_PRE_NON_NULL(type, "Field type");
	BT_ASSERT(field_type_common_has_known_id((void *) type));
	BT_ASSERT_PRE(bt_field_type_common_validate((void *) type) == 0,
		"Field type is invalid: %!+F", type);
	type_id = bt_field_type_get_type_id(type);
	field = field_create_funcs[type_id](type);
	if (!field) {
		goto end;
	}

	bt_field_type_freeze(type);

end:
	return field;
}

struct bt_field_type *bt_field_borrow_type(struct bt_field *field)
{
	return (void *) bt_field_common_borrow_type((void *) field);
}

enum bt_field_type_id bt_field_get_type_id(struct bt_field *field)
{
	struct bt_field_common *field_common = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Field");
	return field_common->type->id;
}

bt_bool bt_field_is_integer(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_INTEGER;
}

bt_bool bt_field_is_floating_point(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_FLOAT;
}

bt_bool bt_field_is_enumeration(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_ENUM;
}

bt_bool bt_field_is_string(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_STRING;
}

bt_bool bt_field_is_structure(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_STRUCT;
}

bt_bool bt_field_is_array(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_ARRAY;
}

bt_bool bt_field_is_sequence(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_SEQUENCE;
}

bt_bool bt_field_is_variant(struct bt_field *field)
{
	return bt_field_get_type_id(field) == BT_FIELD_TYPE_ID_VARIANT;
}

int64_t bt_field_sequence_get_length(struct bt_field *field)
{
	return bt_field_common_sequence_get_length((void *) field);
}

int bt_field_sequence_set_length(struct bt_field *field, uint64_t length)
{
	return bt_field_common_sequence_set_length((void *) field,
		length, (bt_field_common_create_func) bt_field_create_recursive);
}

struct bt_field *bt_field_structure_borrow_field_by_index(
		struct bt_field *field, uint64_t index)
{
	return (void *) bt_field_common_structure_borrow_field_by_index(
		(void *) field, index);
}

struct bt_field *bt_field_structure_borrow_field_by_name(
		struct bt_field *field, const char *name)
{
	return (void *) bt_field_common_structure_borrow_field_by_name(
		(void *) field, name);
}

struct bt_field *bt_field_array_borrow_field(
		struct bt_field *field, uint64_t index)
{
	return (void *) bt_field_common_array_borrow_field((void *) field,
		index);
}

struct bt_field *bt_field_sequence_borrow_field(
		struct bt_field *field, uint64_t index)
{
	return (void *) bt_field_common_sequence_borrow_field((void *) field,
		index);
}

struct bt_field *bt_field_variant_borrow_current_field(
		struct bt_field *variant_field)
{
	return (void *) bt_field_common_variant_borrow_current_field(
		(void *) variant_field);
}

int bt_field_variant_set_tag_signed(struct bt_field *variant_field,
		int64_t tag)
{
	return bt_field_variant_common_set_tag((void *) variant_field,
		(uint64_t) tag, true);
}

int bt_field_variant_set_tag_unsigned(struct bt_field *variant_field,
		uint64_t tag)
{
	return bt_field_variant_common_set_tag((void *) variant_field,
		(uint64_t) tag, false);
}

int bt_field_variant_get_tag_signed(struct bt_field *variant_field,
		int64_t *tag)
{
	return bt_field_common_variant_get_tag_signed((void *) variant_field, tag);
}

int bt_field_variant_get_tag_unsigned(struct bt_field *variant_field,
		uint64_t *tag)
{
	return bt_field_common_variant_get_tag_unsigned((void *) variant_field, tag);
}

struct bt_field_type_enumeration_mapping_iterator *
bt_field_enumeration_get_mappings(struct bt_field *field)
{
	struct bt_field_enumeration *enum_field = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Enumeration field");
	BT_ASSERT_PRE_FIELD_COMMON_HAS_TYPE_ID((struct bt_field_common *) field,
		BT_FIELD_TYPE_ID_ENUM, "Field");
	BT_ASSERT_PRE_FIELD_COMMON_IS_SET((struct bt_field_common *) field,
		"Enumeration field");
	return bt_field_common_enumeration_get_mappings((void *) field,
		(bt_field_common_create_func) bt_field_create_recursive,
		enum_field->common.payload.unsignd);
}

BT_ASSERT_PRE_FUNC
static inline
struct bt_field_type_common_integer *get_int_enum_int_ft(
		struct bt_field *field)
{
	struct bt_field_common_integer *int_field = (void *) field;
	struct bt_field_type_common_integer *int_ft = NULL;

	if (int_field->common.type->id == BT_FIELD_TYPE_ID_INTEGER) {
		int_ft = BT_FROM_COMMON(int_field->common.type);
	} else if (int_field->common.type->id == BT_FIELD_TYPE_ID_ENUM) {
		struct bt_field_type_common_enumeration *enum_ft =
			BT_FROM_COMMON(int_field->common.type);
		int_ft = enum_ft->container_ft;
	}

	BT_ASSERT(int_ft);
	return int_ft;
}

int bt_field_integer_signed_get_value(struct bt_field *field, int64_t *value)
{
	struct bt_field_common_integer *integer = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Integer/enumeration field");
	BT_ASSERT_PRE_NON_NULL(value, "Value");
	BT_ASSERT_PRE_FIELD_COMMON_IS_SET(BT_TO_COMMON(integer),
		"Integer/enumeration field");
	BT_ASSERT_PRE_FIELD_IS_INT_OR_ENUM(BT_TO_COMMON(integer), "Field");
	BT_ASSERT_PRE(bt_field_type_common_integer_is_signed(
		BT_TO_COMMON(get_int_enum_int_ft(field))),
		"Field's type is unsigned: %!+f", field);
	*value = integer->payload.signd;
	return 0;
}

int bt_field_integer_signed_set_value(struct bt_field *field, int64_t value)
{
	int ret = 0;
	struct bt_field_common_integer *integer = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Integer field");
	BT_ASSERT_PRE_FIELD_COMMON_HOT(BT_TO_COMMON(integer), "Integer field");
	BT_ASSERT_PRE_FIELD_IS_INT_OR_ENUM(BT_TO_COMMON(integer), "Field");
	BT_ASSERT_PRE(bt_field_type_common_integer_is_signed(
		BT_FROM_COMMON(get_int_enum_int_ft(field))),
		"Field's type is unsigned: %!+f", field);
	BT_ASSERT_PRE(value_is_in_range_signed(
		get_int_enum_int_ft(field)->size, value),
		"Value is out of bounds: value=%" PRId64 ", %![field-]+f",
		value, field);
	integer->payload.signd = value;
	bt_field_set(field, true);
	return ret;
}

int bt_field_integer_unsigned_get_value(struct bt_field *field, uint64_t *value)
{
	struct bt_field_common_integer *integer = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Integer field");
	BT_ASSERT_PRE_NON_NULL(value, "Value");
	BT_ASSERT_PRE_FIELD_COMMON_IS_SET(BT_TO_COMMON(integer), "Integer field");
	BT_ASSERT_PRE_FIELD_IS_INT_OR_ENUM(BT_TO_COMMON(integer), "Field");
	BT_ASSERT_PRE(!bt_field_type_common_integer_is_signed(
		BT_FROM_COMMON(get_int_enum_int_ft(field))),
		"Field's type is signed: %!+f", field);
	*value = integer->payload.unsignd;
	return 0;
}

int bt_field_integer_unsigned_set_value(struct bt_field *field,
		uint64_t value)
{
	struct bt_field_common_integer *integer = (void *) field;

	BT_ASSERT_PRE_NON_NULL(field, "Integer field");
	BT_ASSERT_PRE_FIELD_COMMON_HOT(BT_TO_COMMON(integer), "Integer field");
	BT_ASSERT_PRE_FIELD_IS_INT_OR_ENUM(BT_TO_COMMON(integer), "Field");
	BT_ASSERT_PRE(!bt_field_type_common_integer_is_signed(
		BT_FROM_COMMON(get_int_enum_int_ft(field))),
		"Field's type is signed: %!+f", field);
	BT_ASSERT_PRE(value_is_in_range_unsigned(
		get_int_enum_int_ft(field)->size, value),
		"Value is out of bounds: value=%" PRIu64 ", %![field-]+f",
		value, field);
	integer->payload.unsignd = value;
	bt_field_set(field, true);
	return 0;
}

int bt_field_floating_point_get_value(struct bt_field *field,
		double *value)
{
	return bt_field_common_floating_point_get_value((void *) field, value);
}

int bt_field_floating_point_set_value(struct bt_field *field,
		double value)
{
	return bt_field_common_floating_point_set_value((void *) field, value);
}

const char *bt_field_string_get_value(struct bt_field *field)
{
	return bt_field_common_string_get_value((void *) field);
}

int bt_field_string_set_value(struct bt_field *field, const char *value)
{
	return bt_field_common_string_set_value((void *) field, value);
}

int bt_field_string_append(struct bt_field *field, const char *value)
{
	return bt_field_common_string_append((void *) field, value);
}

int bt_field_string_append_len(struct bt_field *field,
		const char *value, unsigned int length)
{
	return bt_field_common_string_append_len((void *) field, value, length);
}

int bt_field_string_clear(struct bt_field *string_field)
{
	return bt_field_common_string_clear((void *) string_field);
}

BT_HIDDEN
struct bt_field_common *bt_field_common_copy(struct bt_field_common *field)
{
	struct bt_field_common *copy = NULL;

	BT_ASSERT_PRE_NON_NULL(field, "Field");
	BT_ASSERT(field_type_common_has_known_id(field->type));
	BT_ASSERT(field->methods->copy);
	copy = field->methods->copy(field);
	if (!copy) {
		BT_LOGW("Cannot create field: ft-addr=%p", field->type);
		goto end;
	}

	bt_field_common_set(copy, field->payload_set);

end:
	return copy;
}

static
void bt_field_integer_destroy(struct bt_field *field)
{
	BT_LOGD("Destroying integer field object: addr=%p", field);
	bt_field_common_integer_finalize((void *) field);
	g_free(field);
}

static
void bt_field_floating_point_destroy(struct bt_field *field)
{
	BT_LOGD("Destroying floating point field object: addr=%p", field);
	bt_field_common_floating_point_finalize((void *) field);
	g_free(field);
}

static
void bt_field_enumeration_destroy(struct bt_field *field)
{
	BT_LOGD("Destroying enumeration field object: addr=%p", field);
	bt_field_common_finalize((void *) field);
	g_free(field);
}

static
void bt_field_structure_destroy_recursive(struct bt_field *field)
{
	BT_LOGD("Destroying structure field object: addr=%p", field);
	bt_field_common_structure_finalize_recursive((void *) field);
	g_free(field);
}

static
void bt_field_variant_destroy_recursive(struct bt_field *field)
{
	BT_LOGD("Destroying variant field object: addr=%p", field);
	bt_field_common_variant_finalize_recursive((void *) field);
	g_free(field);
}

static
void bt_field_array_destroy_recursive(struct bt_field *field)
{
	BT_LOGD("Destroying array field object: addr=%p", field);
	bt_field_common_array_finalize_recursive((void *) field);
	g_free(field);
}

static
void bt_field_sequence_destroy_recursive(struct bt_field *field)
{
	BT_LOGD("Destroying sequence field object: addr=%p", field);
	bt_field_common_sequence_finalize_recursive((void *) field);
	g_free(field);
}

static
void bt_field_string_destroy(struct bt_field *field)
{
	BT_LOGD("Destroying string field object: addr=%p", field);
	bt_field_common_string_finalize((void *) field);
	g_free(field);
}

BT_HIDDEN
void bt_field_destroy_recursive(struct bt_field *field)
{
	struct bt_field_common *field_common = (void *) field;

	if (!field) {
		return;
	}

	BT_ASSERT(field_type_common_has_known_id((void *) field_common->type));
	field_destroy_funcs[field_common->type->id](field);
}

static
struct bt_field *bt_field_integer_create(struct bt_field_type *type)
{
	struct bt_field_common_integer *integer =
		g_new0(struct bt_field_common_integer, 1);

	BT_LOGD("Creating integer field object: ft-addr=%p", type);

	if (integer) {
		bt_field_common_initialize(BT_TO_COMMON(integer), (void *) type,
			false, NULL, &bt_field_integer_methods);
		BT_LOGD("Created integer field object: addr=%p, ft-addr=%p",
			integer, type);
	} else {
		BT_LOGE_STR("Failed to allocate one integer field.");
	}

	return (void *) integer;
}

static
struct bt_field *bt_field_enumeration_create(struct bt_field_type *type)
{
	struct bt_field_enumeration *enumeration = g_new0(
		struct bt_field_enumeration, 1);

	BT_LOGD("Creating enumeration field object: ft-addr=%p", type);

	if (enumeration) {
		bt_field_common_initialize(
			BT_TO_COMMON(BT_TO_COMMON(enumeration)),
			(void *) type, false, NULL,
			&bt_field_enumeration_methods);
		BT_LOGD("Created enumeration field object: addr=%p, ft-addr=%p",
			enumeration, type);
	} else {
		BT_LOGE_STR("Failed to allocate one enumeration field.");
	}

	return (void *) enumeration;
}

static
struct bt_field *bt_field_floating_point_create(struct bt_field_type *type)
{
	struct bt_field_common_floating_point *floating_point;

	BT_LOGD("Creating floating point number field object: ft-addr=%p", type);
	floating_point = g_new0(struct bt_field_common_floating_point, 1);

	if (floating_point) {
		bt_field_common_initialize(BT_TO_COMMON(floating_point),
			(void *) type, false, NULL,
			&bt_field_floating_point_methods);
		BT_LOGD("Created floating point number field object: addr=%p, ft-addr=%p",
			floating_point, type);
	} else {
		BT_LOGE_STR("Failed to allocate one floating point number field.");
	}

	return (void *) floating_point;
}

BT_HIDDEN
int bt_field_common_structure_initialize(struct bt_field_common *field,
		struct bt_field_type_common *type,
		bool is_shared, bt_object_release_func release_func,
		struct bt_field_common_methods *methods,
		bt_field_common_create_func field_create_func,
		GDestroyNotify field_release_func)
{
	int ret = 0;
	struct bt_field_type_common_structure *structure_type =
		BT_FROM_COMMON(type);
	struct bt_field_common_structure *structure = BT_FROM_COMMON(field);
	size_t i;

	BT_LOGD("Initializing common structure field object: ft-addr=%p", type);
	bt_field_common_initialize(field, type, is_shared,
		release_func, methods);
	structure->fields = g_ptr_array_new_with_free_func(field_release_func);
	g_ptr_array_set_size(structure->fields, structure_type->fields->len);

	/* Create all fields contained in the structure field. */
	for (i = 0; i < structure_type->fields->len; i++) {
		struct bt_field_common *field;
		struct bt_field_type_common_structure_field *struct_field =
			BT_FIELD_TYPE_COMMON_STRUCTURE_FIELD_AT_INDEX(
				structure_type, i);
		field = field_create_func(struct_field->type);
		if (!field) {
			BT_LOGE("Failed to create structure field's member: name=\"%s\", index=%zu",
				g_quark_to_string(struct_field->name), i);
			ret = -1;
			goto end;
		}

		g_ptr_array_index(structure->fields, i) = field;
	}

	BT_LOGD("Initialized common structure field object: addr=%p, ft-addr=%p",
		field, type);

end:
	return ret;
}

static
struct bt_field *bt_field_structure_create(struct bt_field_type *type)
{
	struct bt_field_common_structure *structure = g_new0(
		struct bt_field_common_structure, 1);
	int iret;

	BT_LOGD("Creating structure field object: ft-addr=%p", type);

	if (!structure) {
		BT_LOGE_STR("Failed to allocate one structure field.");
		goto end;
	}

	iret = bt_field_common_structure_initialize(BT_TO_COMMON(structure),
		(void *) type, false, NULL, &bt_field_structure_methods,
		(bt_field_common_create_func) bt_field_create_recursive,
		(GDestroyNotify) bt_field_destroy_recursive);
	if (iret) {
		BT_PUT(structure);
		goto end;
	}

	BT_LOGD("Created structure field object: addr=%p, ft-addr=%p",
		structure, type);

end:
	return (void *) structure;
}

BT_HIDDEN
int bt_field_common_variant_initialize(struct bt_field_common *field,
		struct bt_field_type_common *type,
		bool is_shared, bt_object_release_func release_func,
		struct bt_field_common_methods *methods,
		bt_field_common_create_func field_create_func,
		GDestroyNotify field_release_func)
{
	int ret = 0;
	struct bt_field_type_common_variant *variant_type =
		BT_FROM_COMMON(type);
	struct bt_field_common_variant *variant = BT_FROM_COMMON(field);
	size_t i;

	BT_LOGD("Initializing common variant field object: ft-addr=%p", type);
	bt_field_common_initialize(field, type, is_shared,
		release_func, methods);
	ret = bt_field_type_common_variant_update_choices(type);
	if (ret) {
		BT_LOGE("Cannot update common variant field type choices: "
			"ret=%d", ret);
		goto end;
	}

	variant->fields = g_ptr_array_new_with_free_func(field_release_func);
	g_ptr_array_set_size(variant->fields, variant_type->choices->len);

	/* Create all fields contained in the variant field. */
	for (i = 0; i < variant_type->choices->len; i++) {
		struct bt_field_common *field;
		struct bt_field_type_common_variant_choice *var_choice =
			BT_FIELD_TYPE_COMMON_VARIANT_CHOICE_AT_INDEX(
				variant_type, i);

		field = field_create_func(var_choice->type);
		if (!field) {
			BT_LOGE("Failed to create variant field's member: name=\"%s\", index=%zu",
				g_quark_to_string(var_choice->name), i);
			ret = -1;
			goto end;
		}

		g_ptr_array_index(variant->fields, i) = field;
	}

	BT_LOGD("Initialized common variant field object: addr=%p, ft-addr=%p",
		field, type);

end:
	return ret;
}

BT_HIDDEN
int bt_field_common_string_initialize(struct bt_field_common *field,
		struct bt_field_type_common *type,
		bool is_shared, bt_object_release_func release_func,
		struct bt_field_common_methods *methods)
{
	int ret = 0;
	struct bt_field_common_string *string = BT_FROM_COMMON(field);

	BT_LOGD("Initializing common string field object: ft-addr=%p", type);
	bt_field_common_initialize(field, type, is_shared,
		release_func, methods);
	string->buf = g_array_sized_new(FALSE, FALSE, sizeof(char), 1);
	if (!string->buf) {
		ret = -1;
		goto end;
	}

	g_array_index(string->buf, char, 0) = '\0';
	BT_LOGD("Initialized common string field object: addr=%p, ft-addr=%p",
		field, type);

end:
	return ret;
}

static
struct bt_field *bt_field_variant_create(struct bt_field_type *type)
{
	struct bt_field_common_variant *variant = g_new0(
		struct bt_field_common_variant, 1);
	int iret;

	BT_LOGD("Creating variant field object: ft-addr=%p", type);

	if (!variant) {
		BT_LOGE_STR("Failed to allocate one variant field.");
		goto end;
	}

	iret = bt_field_common_variant_initialize(BT_TO_COMMON(variant),
		(void *) type, false, NULL, &bt_field_variant_methods,
		(bt_field_common_create_func) bt_field_create_recursive,
		(GDestroyNotify) bt_field_destroy_recursive);
	if (iret) {
		BT_PUT(variant);
		goto end;
	}

	BT_LOGD("Created variant field object: addr=%p, ft-addr=%p",
		variant, type);

end:
	return (void *) variant;
}

BT_HIDDEN
int bt_field_common_array_initialize(struct bt_field_common *field,
		struct bt_field_type_common *type,
		bool is_shared, bt_object_release_func release_func,
		struct bt_field_common_methods *methods,
		bt_field_common_create_func field_create_func,
		GDestroyNotify field_destroy_func)
{
	struct bt_field_type_common_array *array_type = BT_FROM_COMMON(type);
	struct bt_field_common_array *array = BT_FROM_COMMON(field);
	unsigned int array_length;
	int ret = 0;
	uint64_t i;

	BT_LOGD("Initializing common array field object: ft-addr=%p", type);
	BT_ASSERT(type);
	bt_field_common_initialize(field, type, is_shared,
		release_func, methods);
	array_length = array_type->length;
	array->elements = g_ptr_array_sized_new(array_length);
	if (!array->elements) {
		ret = -1;
		goto end;
	}

	g_ptr_array_set_free_func(array->elements, field_destroy_func);
	g_ptr_array_set_size(array->elements, array_length);

	for (i = 0; i < array_length; i++) {
		array->elements->pdata[i] = field_create_func(
			array_type->element_ft);
		if (!array->elements->pdata[i]) {
			ret = -1;
			goto end;
		}
	}

	BT_LOGD("Initialized common array field object: addr=%p, ft-addr=%p",
		field, type);

end:
	return ret;
}

static
struct bt_field *bt_field_array_create(struct bt_field_type *type)
{
	struct bt_field_common_array *array =
		g_new0(struct bt_field_common_array, 1);
	int ret;

	BT_LOGD("Creating array field object: ft-addr=%p", type);
	BT_ASSERT(type);

	if (!array) {
		BT_LOGE_STR("Failed to allocate one array field.");
		goto end;
	}

	ret = bt_field_common_array_initialize(BT_TO_COMMON(array),
		(void *) type, false, NULL, &bt_field_array_methods,
		(bt_field_common_create_func) bt_field_create_recursive,
		(GDestroyNotify) bt_field_destroy_recursive);
	if (ret) {
		BT_PUT(array);
		goto end;
	}

	BT_LOGD("Created array field object: addr=%p, ft-addr=%p",
		array, type);

end:
	return (void *) array;
}

BT_HIDDEN
int bt_field_common_sequence_initialize(struct bt_field_common *field,
		struct bt_field_type_common *type,
		bool is_shared, bt_object_release_func release_func,
		struct bt_field_common_methods *methods,
		GDestroyNotify field_destroy_func)
{
	struct bt_field_common_sequence *sequence = BT_FROM_COMMON(field);
	int ret = 0;

	BT_LOGD("Initializing common sequence field object: ft-addr=%p", type);
	BT_ASSERT(type);
	bt_field_common_initialize(field, type, is_shared,
		release_func, methods);
	sequence->elements = g_ptr_array_new();
	if (!sequence->elements) {
		ret = -1;
		goto end;
	}

	g_ptr_array_set_free_func(sequence->elements, field_destroy_func);
	BT_LOGD("Initialized common sequence field object: addr=%p, ft-addr=%p",
		field, type);

end:
	return ret;
}

static
struct bt_field *bt_field_sequence_create(struct bt_field_type *type)
{
	struct bt_field_common_sequence *sequence =
		g_new0(struct bt_field_common_sequence, 1);
	int ret;

	BT_LOGD("Creating sequence field object: ft-addr=%p", type);
	BT_ASSERT(type);

	if (!sequence) {
		BT_LOGE_STR("Failed to allocate one sequence field.");
		goto end;
	}

	ret = bt_field_common_sequence_initialize(BT_TO_COMMON(sequence),
		(void *) type, false, NULL, &bt_field_sequence_methods,
		(GDestroyNotify) bt_field_destroy_recursive);
	if (ret) {
		BT_PUT(sequence);
		goto end;
	}

	BT_LOGD("Created sequence field object: addr=%p, ft-addr=%p",
		sequence, type);

end:
	return (void *) sequence;
}

static
struct bt_field *bt_field_string_create(struct bt_field_type *type)
{
	struct bt_field_common_string *string = g_new0(
		struct bt_field_common_string, 1);

	BT_LOGD("Creating string field object: ft-addr=%p", type);

	if (string) {
		bt_field_common_string_initialize(BT_TO_COMMON(string),
			(void *) type, false, NULL, &bt_field_string_methods);
		BT_LOGD("Created string field object: addr=%p, ft-addr=%p",
			string, type);
	} else {
		BT_LOGE_STR("Failed to allocate one string field.");
	}

	return (void *) string;
}

BT_HIDDEN
int bt_field_common_generic_validate(struct bt_field_common *field)
{
	return (field && field->payload_set) ? 0 : -1;
}

BT_HIDDEN
int bt_field_common_structure_validate_recursive(struct bt_field_common *field)
{
	int64_t i;
	int ret = 0;
	struct bt_field_common_structure *structure = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < structure->fields->len; i++) {
		ret = bt_field_common_validate_recursive(
			(void *) structure->fields->pdata[i]);

		if (ret) {
			int this_ret;
			const char *name;

			this_ret = bt_field_type_common_structure_borrow_field_by_index(
				field->type, &name, NULL, i);
			BT_ASSERT(this_ret == 0);
			BT_ASSERT_PRE_MSG("Invalid structure field's field: "
				"%![struct-field-]+_f, field-name=\"%s\", "
				"index=%" PRId64 ", %![field-]+_f",
				field, name, i, structure->fields->pdata[i]);
			goto end;
		}
	}

end:
	return ret;
}

BT_HIDDEN
int bt_field_common_variant_validate_recursive(struct bt_field_common *field)
{
	int ret = 0;
	struct bt_field_common_variant *variant = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	if (!variant->current_field) {
		ret = -1;
		goto end;
	}

	ret = bt_field_common_validate_recursive(variant->current_field);

end:
	return ret;
}

BT_HIDDEN
int bt_field_common_array_validate_recursive(struct bt_field_common *field)
{
	int64_t i;
	int ret = 0;
	struct bt_field_common_array *array = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < array->elements->len; i++) {
		ret = bt_field_common_validate_recursive((void *) array->elements->pdata[i]);
		if (ret) {
			BT_ASSERT_PRE_MSG("Invalid array field's element field: "
				"%![array-field-]+_f, " PRId64 ", "
				"%![elem-field-]+_f",
				field, i, array->elements->pdata[i]);
			goto end;
		}
	}

end:
	return ret;
}

BT_HIDDEN
int bt_field_common_sequence_validate_recursive(struct bt_field_common *field)
{
	size_t i;
	int ret = 0;
	struct bt_field_common_sequence *sequence = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < sequence->elements->len; i++) {
		ret = bt_field_common_validate_recursive(
			(void *) sequence->elements->pdata[i]);
		if (ret) {
			BT_ASSERT_PRE_MSG("Invalid sequence field's element field: "
				"%![seq-field-]+_f, " PRId64 ", "
				"%![elem-field-]+_f",
				field, i, sequence->elements->pdata[i]);
			goto end;
		}
	}
end:
	return ret;
}

BT_HIDDEN
void bt_field_common_generic_reset(struct bt_field_common *field)
{
	BT_ASSERT(field);
	field->payload_set = false;
}

BT_HIDDEN
void bt_field_common_structure_reset_recursive(struct bt_field_common *field)
{
	int64_t i;
	struct bt_field_common_structure *structure = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < structure->fields->len; i++) {
		struct bt_field_common *member = structure->fields->pdata[i];

		if (!member) {
			/*
			 * Structure members are lazily initialized;
			 * skip if this member has not been allocated
			 * yet.
			 */
			continue;
		}

		bt_field_common_reset_recursive(member);
	}
}

BT_HIDDEN
void bt_field_common_variant_reset_recursive(struct bt_field_common *field)
{
	struct bt_field_common_variant *variant = BT_FROM_COMMON(field);

	BT_ASSERT(field);
	variant->current_field = NULL;
}

BT_HIDDEN
void bt_field_common_array_reset_recursive(struct bt_field_common *field)
{
	size_t i;
	struct bt_field_common_array *array = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < array->elements->len; i++) {
		struct bt_field_common *member = array->elements->pdata[i];

		if (!member) {
			/*
			 * Array elements are lazily initialized; skip
			 * if this member has not been allocated yet.
			 */
			continue;
		}

		bt_field_common_reset_recursive(member);
	}
}

BT_HIDDEN
void bt_field_common_sequence_reset_recursive(struct bt_field_common *field)
{
	struct bt_field_common_sequence *sequence = BT_FROM_COMMON(field);
	uint64_t i;

	BT_ASSERT(field);

	for (i = 0; i < sequence->elements->len; i++) {
		if (sequence->elements->pdata[i]) {
			bt_field_common_reset_recursive(
				sequence->elements->pdata[i]);
		}
	}

	sequence->length = 0;
}

BT_HIDDEN
void bt_field_common_generic_set_is_frozen(struct bt_field_common *field,
		bool is_frozen)
{
	field->frozen = is_frozen;
}

BT_HIDDEN
void bt_field_common_structure_set_is_frozen_recursive(
		struct bt_field_common *field, bool is_frozen)
{
	uint64_t i;
	struct bt_field_common_structure *structure_field =
		BT_FROM_COMMON(field);

	BT_LOGD("Freezing structure field object: addr=%p", field);

	for (i = 0; i < structure_field->fields->len; i++) {
		struct bt_field_common *struct_field =
			g_ptr_array_index(structure_field->fields, i);

		BT_LOGD("Freezing structure field's field: field-addr=%p, index=%" PRId64,
			struct_field, i);
		bt_field_common_set_is_frozen_recursive(struct_field,
			is_frozen);
	}

	bt_field_common_generic_set_is_frozen(field, is_frozen);
}

BT_HIDDEN
void bt_field_common_variant_set_is_frozen_recursive(
		struct bt_field_common *field, bool is_frozen)
{
	uint64_t i;
	struct bt_field_common_variant *variant_field = BT_FROM_COMMON(field);

	BT_LOGD("Freezing variant field object: addr=%p", field);

	for (i = 0; i < variant_field->fields->len; i++) {
		struct bt_field_common *var_field =
			g_ptr_array_index(variant_field->fields, i);

		BT_LOGD("Freezing variant field's field: field-addr=%p, index=%" PRId64,
			var_field, i);
		bt_field_common_set_is_frozen_recursive(var_field, is_frozen);
	}

	bt_field_common_generic_set_is_frozen(field, is_frozen);
}

BT_HIDDEN
void bt_field_common_array_set_is_frozen_recursive(
		struct bt_field_common *field, bool is_frozen)
{
	int64_t i;
	struct bt_field_common_array *array_field = BT_FROM_COMMON(field);

	BT_LOGD("Freezing array field object: addr=%p", field);

	for (i = 0; i < array_field->elements->len; i++) {
		struct bt_field_common *elem_field =
			g_ptr_array_index(array_field->elements, i);

		BT_LOGD("Freezing array field object's element field: "
			"element-field-addr=%p, index=%" PRId64,
			elem_field, i);
		bt_field_common_set_is_frozen_recursive(elem_field, is_frozen);
	}

	bt_field_common_generic_set_is_frozen(field, is_frozen);
}

BT_HIDDEN
void bt_field_common_sequence_set_is_frozen_recursive(
		struct bt_field_common *field, bool is_frozen)
{
	int64_t i;
	struct bt_field_common_sequence *sequence_field =
		BT_FROM_COMMON(field);

	BT_LOGD("Freezing sequence field object: addr=%p", field);

	for (i = 0; i < sequence_field->length; i++) {
		struct bt_field_common *elem_field =
			g_ptr_array_index(sequence_field->elements, i);

		BT_LOGD("Freezing sequence field object's element field: "
			"element-field-addr=%p, index=%" PRId64,
			elem_field, i);
		bt_field_common_set_is_frozen_recursive(elem_field, is_frozen);
	}

	bt_field_common_generic_set_is_frozen(field, is_frozen);
}

BT_HIDDEN
void _bt_field_common_set_is_frozen_recursive(struct bt_field_common *field,
		bool is_frozen)
{
	if (!field) {
		goto end;
	}

	if (field->frozen) {
		goto end;
	}

	BT_LOGD("Setting field object's frozen state: addr=%p, is-frozen=%d",
		field, is_frozen);
	BT_ASSERT(field_type_common_has_known_id(field->type));
	BT_ASSERT(field->methods->set_is_frozen);
	field->methods->set_is_frozen(field, is_frozen);

end:
	return;
}

BT_HIDDEN
bt_bool bt_field_common_generic_is_set(struct bt_field_common *field)
{
	return field && field->payload_set;
}

BT_HIDDEN
bt_bool bt_field_common_structure_is_set_recursive(
		struct bt_field_common *field)
{
	bt_bool is_set = BT_FALSE;
	size_t i;
	struct bt_field_common_structure *structure = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < structure->fields->len; i++) {
		is_set = bt_field_common_is_set_recursive(
			structure->fields->pdata[i]);
		if (!is_set) {
			goto end;
		}
	}

end:
	return is_set;
}

BT_HIDDEN
bt_bool bt_field_common_variant_is_set_recursive(struct bt_field_common *field)
{
	struct bt_field_common_variant *variant = BT_FROM_COMMON(field);
	bt_bool is_set = BT_FALSE;

	BT_ASSERT(field);

	if (variant->current_field) {
		is_set = bt_field_common_is_set_recursive(
			variant->current_field);
	}

	return is_set;
}

BT_HIDDEN
bt_bool bt_field_common_array_is_set_recursive(struct bt_field_common *field)
{
	size_t i;
	bt_bool is_set = BT_FALSE;
	struct bt_field_common_array *array = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	for (i = 0; i < array->elements->len; i++) {
		is_set = bt_field_common_is_set_recursive(array->elements->pdata[i]);
		if (!is_set) {
			goto end;
		}
	}

end:
	return is_set;
}

BT_HIDDEN
bt_bool bt_field_common_sequence_is_set_recursive(struct bt_field_common *field)
{
	size_t i;
	bt_bool is_set = BT_FALSE;
	struct bt_field_common_sequence *sequence = BT_FROM_COMMON(field);

	BT_ASSERT(field);

	if (!sequence->elements) {
		goto end;
	}

	for (i = 0; i < sequence->elements->len; i++) {
		is_set = bt_field_common_is_set_recursive(
			sequence->elements->pdata[i]);
		if (!is_set) {
			goto end;
		}
	}

end:
	return is_set;
}
