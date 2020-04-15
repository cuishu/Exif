#ifndef LIST_H
#define LIST_H

#ifndef	offsetof
# define	offsetof(TYPE, MEMBER)		((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef	getaddr
# define	getaddr(ptr, TYPE, MEMBER)	((size_t)(ptr)-offsetof(TYPE, MEMBER))
#endif

#ifndef	getobj
# define	getobj(ptr, TYPE, MEMBER)	((TYPE*)((size_t)(ptr)-offsetof(TYPE, MEMBER)))
#endif

#ifndef	container_of
# define	container_of(ptr, type, member) ({                      \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void node_add(struct list_head *new_node, struct list_head *prev, struct list_head *next)
{
	new_node->next = next;
	new_node->prev = prev;
	prev->next = new_node;
	next->prev = new_node;
}

static inline void node_del(struct list_head *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

static inline void list_append(struct list_head *new_node, struct list_head *head)
{
	new_node->next = head;
	new_node->prev = head->prev;
	head->prev = new_node;
	new_node->prev->next = new_node;
}
#endif
