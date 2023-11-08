// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>

MODULE_AUTHOR("Alina Barynina <alina00427@gmail.com>");
MODULE_DESCRIPTION("Hello, my first module");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int msg_count = 1;
module_param(msg_count, uint, 0444); //Визначення параметра
MODULE_PARM_DESC(msg_count, "Number of times to print 'Hello, world!'");
//Опис параметра

struct datastr {
	struct list_head list;
	ktime_t time;
}; //структура даних

static LIST_HEAD(event_list);
	//Статична змінна голови списку

static int __init hello_init(void)
{
	if (msg_count == 0 || (msg_count >= 5 && msg_count <= 10)) {
		pr_warn("Warning: count of messages is 0 or between 5 and 10, continuing...\n");
	} else if (msg_count > 10) {
		pr_err("Error: count of messages is greater than 10, module cannot be loaded\n");
		return -EINVAL;
	} //Перевірки вхідних даних

	int i;

	for (i = 0; i < msg_count; i++) {
		struct datastr *event = kmalloc(sizeof(struct datastr),
						GFP_KERNEL);

		event->time = ktime_get();

		list_add_tail(&event->list, &event_list);
		pr_emerg("Hello, world!\n");
	} //Цикл для виведення прінтів

	return 0;
}

static void __exit hello_exit(void)
{
	struct datastr *event, *tmp;
	//Виведення часу подій, очищення
	//структури та звільнення пам'яті
	pr_emerg("Events:\n");
	list_for_each_entry_safe(event, tmp, &event_list, list) {
		pr_emerg("Event time: %lld ns\n", ktime_to_ns(event->time));
		list_del(&event->list);
		kfree(event);
	}
}

module_init(hello_init);
module_exit(hello_exit);
