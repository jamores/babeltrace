#ifndef BABELTRACE_GRAPH_NOTIFICATION_INACTIVITY_H
#define BABELTRACE_GRAPH_NOTIFICATION_INACTIVITY_H

/*
 * Copyright 2017 Philippe Proulx <pproulx@efficios.com>
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

/* For bt_get() */
#include <babeltrace/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_notification;
struct bt_clock_class_priority_map;
struct bt_clock_class;

extern struct bt_notification *bt_notification_inactivity_create(
		struct bt_clock_class_priority_map *clock_class_priority_map);

extern struct bt_clock_class_priority_map *
bt_notification_inactivity_borrow_clock_class_priority_map(
		struct bt_notification *notification);

static inline
struct bt_clock_class_priority_map *
bt_notification_inactivity_get_clock_class_priority_map(
		struct bt_notification *notification)
{
	return bt_get(
		bt_notification_inactivity_borrow_clock_class_priority_map(
			notification));
}

extern struct bt_clock_value *bt_notification_inactivity_borrow_clock_value(
		struct bt_notification *notification,
		struct bt_clock_class *clock_class);

static inline
struct bt_clock_value *bt_notification_inactivity_get_clock_value(
		struct bt_notification *notification,
		struct bt_clock_class *clock_class)
{
	return bt_get(bt_notification_inactivity_borrow_clock_value(
		notification, clock_class));
}

extern int bt_notification_inactivity_set_clock_value(
		struct bt_notification *notification,
		struct bt_clock_value *clock_value);

#ifdef __cplusplus
}
#endif

#endif /* BABELTRACE_GRAPH_NOTIFICATION_INACTIVITY_H */