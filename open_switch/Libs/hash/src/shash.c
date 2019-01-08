//#include <libjw/libjw.h>
#include "common.h"
#include "jhash.h"
#include "shash.h"

static struct shash_node *shash_find__(const struct shash *,
                                       const char *name, size_t name_len,
                                       size_t hash);

void
shash_init(struct shash *sh)
{
    hmap_init(&sh->map);
}

void
shash_destroy(struct shash *sh)
{
    if (sh) {
        shash_clear(sh);
        hmap_destroy(&sh->map);
    }
}

/* Like shash_destroy(), but also free() each node's 'data'. */
void
shash_destroy_free_data(struct shash *sh)
{
    if (sh) {
        shash_clear_free_data(sh);
        hmap_destroy(&sh->map);
    }
}

void
shash_swap(struct shash *a, struct shash *b)
{
    hmap_swap(&a->map, &b->map);
}

void
shash_moved(struct shash *sh)
{
    hmap_moved(&sh->map);
}

void
shash_clear(struct shash *sh)
{
    struct shash_node *node, *next;

    SHASH_FOR_EACH_SAFE(node, next, sh) {
        hmap_remove(&sh->map, &node->node);
        free(node->name);
        free(node);
    }
}

/* Like shash_clear(), but also free() each node's 'data'. */
void
shash_clear_free_data(struct shash *sh)
{
    struct shash_node *node, *next;

    SHASH_FOR_EACH_SAFE(node, next, sh) {
        hmap_remove(&sh->map, &node->node);
        free(node->data);
        free(node->name);
        free(node);
    }
}

bool
shash_is_empty(const struct shash *shash)
{
    return hmap_is_empty(&shash->map);
}

size_t
shash_count(const struct shash *shash)
{
    return hmap_count(&shash->map);
}

static struct shash_node *
shash_add_nocopy__(struct shash *sh, char *name, const void *data, size_t hash)
{
    struct shash_node *node = malloc(sizeof *node);
    node->name = name;
    node->data = (void *)data;
    hmap_insert(&sh->map, &node->node, hash);
    return node;
}

/* It is the caller's responsibility to avoid duplicate names, if that is
 * desirable. */
struct shash_node *
shash_add_nocopy(struct shash *sh, char *name, const void *data)
{
    return shash_add_nocopy__(sh, name, data, jhash(name,strlen(name),0));
}

/* It is the caller's responsibility to avoid duplicate names, if that is
 * desirable. */
struct shash_node *
shash_add(struct shash *sh, const char *name, const void *data)
{
    return shash_add_nocopy(sh, strdup(name), data);
}

bool
shash_add_once(struct shash *sh, const char *name, const void *data)
{
    if (!shash_find(sh, name)) {
        shash_add(sh, name, data);
        return true;
    } else {
        return false;
    }
}

/* Searches for 'name' in 'sh'.  If it does not already exist, adds it along
 * with 'data' and returns NULL.  If it does already exist, replaces its data
 * by 'data' and returns the data that it formerly contained. */
void *
shash_replace(struct shash *sh, const char *name, const void *data)
{
    size_t hash = jhash(name,strlen(name),0);
    struct shash_node *node;

    node = shash_find__(sh, name, strlen(name), hash);
    if (!node) {
        shash_add_nocopy__(sh, strdup(name), data, hash);
        return NULL;
    } else {
        void *old_data = node->data;
        node->data = (void *)data;
        return old_data;
    }
}

/* Deletes 'node' from 'sh' and frees the node's name.  The caller is still
 * responsible for freeing the node's data, if necessary. */
void
shash_delete(struct shash *sh, struct shash_node *node)
{
    free(shash_steal(sh, node));
}

/* Deletes 'node' from 'sh'.  Neither the node's name nor its data is freed;
 * instead, ownership is transferred to the caller.  Returns the node's
 * name. */
char *
shash_steal(struct shash *sh, struct shash_node *node)
{
    char *name = node->name;

    hmap_remove(&sh->map, &node->node);
    free(node);
    return name;
}

static struct shash_node *
shash_find__(const struct shash *sh, const char *name, size_t name_len,
             size_t hash)
{
    struct shash_node *node;

    HMAP_FOR_EACH_WITH_HASH (node, node, hash, &sh->map) {
        if (!strncmp(node->name, name, name_len) && !node->name[name_len]) {
            return node;
        }
    }
    return NULL;
}

/* If there are duplicates, returns a random element. */
struct shash_node *
shash_find(const struct shash *sh, const char *name)
{
    return shash_find__(sh, name, strlen(name), jhash(name,strlen(name),0));
}

/* Finds and returns a shash_node within 'sh' that has the given 'name' that is
 * exactly 'len' bytes long.  Returns NULL if no node in 'sh' has that name. */
struct shash_node *
shash_find_len(const struct shash *sh, const char *name, size_t len)
{
    return shash_find__(sh, name, len, jhash(name, len, 0));
}

void *
shash_find_data(const struct shash *sh, const char *name)
{
    struct shash_node *node = shash_find(sh, name);
    return node ? node->data : NULL;
}

void *
shash_find_and_delete(struct shash *sh, const char *name)
{
    struct shash_node *node = shash_find(sh, name);
    if (node) {
        void *data = node->data;
        shash_delete(sh, node);
        return data;
    } else {
        return NULL;
    }
}

struct shash_node *
shash_first(const struct shash *shash)
{
    struct hmap_node *node = hmap_first(&shash->map);
    return node ? container_of(node, struct shash_node, node) : NULL;
}

static int
compare_nodes_by_name(const void *a_, const void *b_)
{
    const struct shash_node *const *a = a_;
    const struct shash_node *const *b = b_;
    return strcmp((*a)->name, (*b)->name);
}

const struct shash_node **
shash_sort(const struct shash *sh)
{
    if (shash_is_empty(sh)) {
        return NULL;
    } else {
        const struct shash_node **nodes;
        struct shash_node *node;
        size_t i, n;

        n = shash_count(sh);
        nodes = malloc(n * sizeof *nodes);
        i = 0;
        SHASH_FOR_EACH (node, sh) {
            nodes[i++] = node;
        }

        qsort(nodes, n, sizeof *nodes, compare_nodes_by_name);

        return nodes;
    }
}

/* Returns true if 'a' and 'b' contain the same keys (regardless of their
 * values), false otherwise. */
bool
shash_equal_keys(const struct shash *a, const struct shash *b)
{
    struct shash_node *node;

    if (hmap_count(&a->map) != hmap_count(&b->map)) {
        return false;
    }
    SHASH_FOR_EACH (node, a) {
        if (!shash_find(b, node->name)) {
            return false;
        }
    }
    return true;
}

