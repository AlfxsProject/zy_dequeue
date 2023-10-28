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
#include "zydequeue.h"
#include <stdatomic.h>
#include <string.h>

typedef struct zydequeue_bx_s
{
    const zyalloc_t *const alloc;
    struct zydequeue_bx_s *previous, *next;
    const opaque_t opaque;
} zydequeue_bx_t;

struct zydequeue_s
{
    const zyalloc_t *const alloc;
    zydequeue_bx_t *first, *last;
    atomic_size_t size;
};

__attribute__((nonnull)) static int zydequeue_bx_construct(zydequeue_bx_t **bx, const zyalloc_t *alloc,
                                                         const opaque_t *opaque)
{
    int r = zymalloc(alloc, sizeof(zydequeue_bx_t) + opaque->size, (void **)bx);
    if (r == ZYALLOC_OK)
    {
        const zydequeue_bx_t bx_init = {.alloc = alloc, .previous = nullptr, .next = nullptr, {.size = opaque->size}};
        memcpy((void *)*bx, (void *)&bx_init, sizeof(zydequeue_bx_t));
        memcpy(((void *)*bx) + sizeof(zydequeue_bx_t), (void *)&opaque->data, opaque->size);
    }
    return r;
}

__attribute__((nonnull)) static void zydequeue_bx_destruct(zydequeue_bx_t **bx)
{
    if (*bx != nullptr)
    {
        zyfree((*bx)->alloc, (void **)bx);
    }
}

int zydequeue_construct(zydequeue_t **zydequeue, const zyalloc_t *alloc)
{
    int r = zymalloc(alloc, sizeof(zydequeue_t), (void **)zydequeue);
    if (r == ZYALLOC_OK)
    {
        const zydequeue_t zydequeue_init = {.alloc = alloc, .first = nullptr, .last = nullptr, .size = 0};
        memcpy((void *)*zydequeue, &zydequeue_init, sizeof(zydequeue_t));
    }
    return r;
}

void zydequeue_destruct(zydequeue_t **zydequeue)
{
    if (*zydequeue != nullptr)
    {
        zydequeue_clear(*zydequeue);
        zyfree((*zydequeue)->alloc, (void **)zydequeue);
    }
}

void zydequeue_clear(zydequeue_t *zydequeue)
{
    zydequeue_bx_t *bx = zydequeue->first;
    while (bx != nullptr)
    {
        zydequeue_bx_t *next = bx->next;
        zydequeue_bx_destruct(&bx);
        bx = next;
    }
    zydequeue->first = nullptr;
    zydequeue->last = nullptr;
    zydequeue->size = 0;
}

int zydequeue_push_first(zydequeue_t *zydequeue, const opaque_t *opaque)
{
    zydequeue_bx_t *bx;
    int r = zydequeue_bx_construct(&bx, zydequeue->alloc, opaque);
    if (r == ZYALLOC_OK)
    {
        if (zydequeue->size != 0)
        {
            bx->next = zydequeue->first;
            zydequeue->first->previous = bx;
            zydequeue->first = bx;
        }
        else
        {
            zydequeue->first = bx;
            zydequeue->last = bx;
        }
        ++zydequeue->size;
    }
    return r;
}

int zydequeue_push_last(zydequeue_t *zydequeue, const opaque_t *opaque)
{
    zydequeue_bx_t *bx;
    int r = zydequeue_bx_construct(&bx, zydequeue->alloc, opaque);
    if (r == ZYALLOC_OK)
    {
        if (zydequeue->size != 0)
        {
            bx->previous = zydequeue->last;
            zydequeue->last->next = bx;
            zydequeue->last = bx;
        }
        else
        {
            zydequeue->first = bx;
            zydequeue->last = bx;
        }
        ++zydequeue->size;
    }
    return r;
}

void zydequeue_discard_first(zydequeue_t *zydequeue)
{
    if (zydequeue->size != 0)
    {
        zydequeue_bx_t *bx = zydequeue->first;
        if (bx->next != nullptr)
        {
            bx->next->previous = nullptr;
            zydequeue->first = bx->next;
        }
        else
        {
            zydequeue->first = nullptr;
            zydequeue->last = nullptr;
        }
        zydequeue_bx_destruct(&bx);
        --zydequeue->size;
    }
}

void zydequeue_discard_last(zydequeue_t *zydequeue)
{
    if (zydequeue->size != 0)
    {
        zydequeue_bx_t *bx = zydequeue->last;
        if (bx->previous != nullptr)
        {
            bx->previous->next = nullptr;
            zydequeue->last = bx->previous;
        }
        else
        {
            zydequeue->first = nullptr;
            zydequeue->last = nullptr;
        }
        zydequeue_bx_destruct(&bx);
        --zydequeue->size;
    }
}

const opaque_t *zydequeue_peek_first(const zydequeue_t *zydequeue)
{
    return &zydequeue->first->opaque;
}

const opaque_t *zydequeue_peek_last(const zydequeue_t *zydequeue)
{
    return &zydequeue->last->opaque;
}

size_t zydequeue_size(const zydequeue_t *zydequeue)
{
    return zydequeue->size;
}

_Bool zydequeue_is_empty(const zydequeue_t *zydequeue)
{
    return zydequeue->size == 0;
}