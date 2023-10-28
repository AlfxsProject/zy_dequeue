/*
 * Copyright 2023 Alexandre Fernandez <alex@fernandezfamily.email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "dequeue.h"
#include <stdatomic.h>
#include <string.h>

typedef struct dequeue_bx_s
{
    const zyalloc_t *const alloc;
    struct dequeue_bx_s *previous, *next;
    const opaque_t opaque;
} dequeue_bx_t;

struct dequeue_s
{
    const zyalloc_t *const alloc;
    dequeue_bx_t *first, *last;
    atomic_size_t size;
};

__attribute__((nonnull)) static int dequeue_bx_construct(dequeue_bx_t **bx, const zyalloc_t *alloc,
                                                         const opaque_t *opaque)
{
    int r = zymalloc(alloc, sizeof(dequeue_bx_t) + opaque->size, (void **)bx);
    if (r == ZYALLOC_OK)
    {
        const dequeue_bx_t bx_init = {.alloc = alloc, .previous = nullptr, .next = nullptr, {.size = opaque->size}};
        memcpy((void *)*bx, (void *)&bx_init, sizeof(dequeue_bx_t));
        memcpy(((void *)*bx) + sizeof(dequeue_bx_t), (void *)&opaque->data, opaque->size);
    }
    return r;
}

__attribute__((nonnull)) static void dequeue_bx_destruct(dequeue_bx_t **bx)
{
    if (*bx != nullptr)
    {
        zyfree((*bx)->alloc, (void **)bx);
    }
}

int dequeue_construct(dequeue_t **dequeue, const zyalloc_t *alloc)
{
    int r = zymalloc(alloc, sizeof(dequeue_t), (void **)dequeue);
    if (r == ZYALLOC_OK)
    {
        const dequeue_t dequeue_init = {.alloc = alloc, .first = nullptr, .last = nullptr, .size = 0};
        memcpy((void *)*dequeue, &dequeue_init, sizeof(dequeue_t));
    }
    return r;
}

void dequeue_destruct(dequeue_t **dequeue)
{
    if (*dequeue != nullptr)
    {
        dequeue_clear(*dequeue);
        zyfree((*dequeue)->alloc, (void **)dequeue);
    }
}

void dequeue_clear(dequeue_t *dequeue)
{
    dequeue_bx_t *bx = dequeue->first;
    while (bx != nullptr)
    {
        dequeue_bx_t *next = bx->next;
        dequeue_bx_destruct(&bx);
        bx = next;
    }
    dequeue->first = nullptr;
    dequeue->last = nullptr;
    dequeue->size = 0;
}

int dequeue_push_first(dequeue_t *dequeue, const opaque_t *opaque)
{
    dequeue_bx_t *bx;
    int r = dequeue_bx_construct(&bx, dequeue->alloc, opaque);
    if (r == ZYALLOC_OK)
    {
        if (dequeue->size != 0)
        {
            bx->next = dequeue->first;
            dequeue->first->previous = bx;
            dequeue->first = bx;
        }
        else
        {
            dequeue->first = bx;
            dequeue->last = bx;
        }
        ++dequeue->size;
    }
    return r;
}

int dequeue_push_last(dequeue_t *dequeue, const opaque_t *opaque)
{
    dequeue_bx_t *bx;
    int r = dequeue_bx_construct(&bx, dequeue->alloc, opaque);
    if (r == ZYALLOC_OK)
    {
        if (dequeue->size != 0)
        {
            bx->previous = dequeue->last;
            dequeue->last->next = bx;
            dequeue->last = bx;
        }
        else
        {
            dequeue->first = bx;
            dequeue->last = bx;
        }
        ++dequeue->size;
    }
    return r;
}

void dequeue_discard_first(dequeue_t *dequeue)
{
    if (dequeue->size != 0)
    {
        dequeue_bx_t *bx = dequeue->first;
        if (bx->next != nullptr)
        {
            bx->next->previous = nullptr;
            dequeue->first = bx->next;
        }
        else
        {
            dequeue->first = nullptr;
            dequeue->last = nullptr;
        }
        dequeue_bx_destruct(&bx);
        --dequeue->size;
    }
}

void dequeue_discard_last(dequeue_t *dequeue)
{
    if (dequeue->size != 0)
    {
        dequeue_bx_t *bx = dequeue->last;
        if (bx->previous != nullptr)
        {
            bx->previous->next = nullptr;
            dequeue->last = bx->previous;
        }
        else
        {
            dequeue->first = nullptr;
            dequeue->last = nullptr;
        }
        dequeue_bx_destruct(&bx);
        --dequeue->size;
    }
}

const opaque_t *dequeue_peek_first(const dequeue_t *dequeue)
{
    return &dequeue->first->opaque;
}

const opaque_t *dequeue_peek_last(const dequeue_t *dequeue)
{
    return &dequeue->last->opaque;
}

size_t dequeue_size(const dequeue_t *dequeue)
{
    return dequeue->size;
}

_Bool dequeue_is_empty(const dequeue_t *dequeue)
{
    return dequeue->size == 0;
}