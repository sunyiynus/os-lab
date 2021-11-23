#ifndef __SMART_POINTER_H__
#define __SMART_POINTER_H__

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>


#define smart __attribute__((cleanup(sfree_stack)))
#define smalloc(size,...) _smalloc(size, __VA_ARGS__)

#define smalloc(size) _smalloc(size,free);
#define smalloc(size, fp) _smalloc(size, fp);

/*
 *
 * smart int* i = smalloc(sizeof(int));
 *
 *
 */


struct meta {
    void (*dtor)(void*);
    void *ptr;
};


static struct meta* get_meta(void* ptr) {
    return ptr-sizeof(struct meta);
}

__attribute__((malloc))
    void* _smalloc(size_t size, void (*dtor)(void*)) {
        struct meta* meta = malloc(sizeof(struct meta) + size);
        *meta = (struct meta) {
            .dtor = dtor,
            .ptr  = meta+1
        };

        return meta->ptr;
    }

void sfree(void* ptr) {
    if(ptr == NULL) return;

    struct meta* meta = get_meta(ptr);
    assert(ptr == meta->ptr);
    meta->dtor(ptr);
    free(meta);
}

__attribute__((always_inline))
    inline void sfree_stack(void* ptr) {
        sfree( *(void**)ptr );
    }


#endif /* __SMART_POINTER_H__ */
