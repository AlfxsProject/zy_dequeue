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
#include "zy_dequeue.h"
#include <stdatomic.h>
#include <string.h>

typedef struct zy_dequeue_bx_s
{
    const zy_alloc_t *alloc;
    struct zy_dequeue_bx_s *previous, *next;
    zy_opaque_t opaque;
} zy_dequeue_bx_t;

struct zy_dequeue_s
{
    const zy_alloc_t *alloc;
    zy_dequeue_bx_t *first, *last;
    atomic_size_t size;
};

__attribute__((nonnull)) static int zy_dequeue_bx_construct(zy_dequeue_bx_t **bx, const zy_alloc_t *alloc,
                                                            const zy_opaque_t *opaque)
{
    int r = zy_malloc(alloc, sizeof(zy_dequeue_bx_t) + opaque->size, (void **)bx);
    if (r == ZY_OK)
    {
        (*bx)->alloc = alloc;
        (*bx)->previous = nullptr;
        (*bx)->next = nullptr;
        memcpy((void *)&(*bx)->opaque, opaque, sizeof(zy_opaque_t));
    }
    return r;
}

__attribute__((nonnull)) static void zy_dequeue_bx_destruct(zy_dequeue_bx_t **bx)
{
    if (*bx != nullptr)
    {
        zy_free((*bx)->alloc, (void **)bx);
    }
}

int zy_dequeue_construct(zy_dequeue_t **dequeue, const zy_alloc_t *alloc)
{
    int r = zy_malloc(alloc, sizeof(zy_dequeue_t), (void **)dequeue);
    if (r == ZY_OK)
    {
        (*dequeue)->alloc = alloc;
        (*dequeue)->first = nullptr;
        (*dequeue)->last = nullptr;
        (*dequeue)->size = 0;
    }
    return r;
}

void zy_dequeue_destruct(zy_dequeue_t **dequeue)
{
    if (*dequeue != nullptr)
    {
        zy_dequeue_clear(*dequeue);
        zy_free((*dequeue)->alloc, (void **)dequeue);
    }
}

void zy_dequeue_clear(zy_dequeue_t *dequeue)
{
    zy_dequeue_bx_t *bx = dequeue->first;
    while (bx != nullptr)
    {
        zy_dequeue_bx_t *next = bx->next;
        zy_dequeue_bx_destruct(&bx);
        bx = next;
    }
    dequeue->first = nullptr;
    dequeue->last = nullptr;
    dequeue->size = 0;
}

int zy_dequeue_push_first(zy_dequeue_t *dequeue, const zy_opaque_t *opaque)
{
    zy_dequeue_bx_t *bx;
    int r = zy_dequeue_bx_construct(&bx, dequeue->alloc, opaque);
    if (r == ZY_OK)
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

int zy_dequeue_push_last(zy_dequeue_t *dequeue, const zy_opaque_t *opaque)
{
    zy_dequeue_bx_t *bx;
    int r = zy_dequeue_bx_construct(&bx, dequeue->alloc, opaque);
    if (r == ZY_OK)
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

void zy_dequeue_discard_first(zy_dequeue_t *dequeue)
{
    if (dequeue->size != 0)
    {
        zy_dequeue_bx_t *bx = dequeue->first;
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
        zy_dequeue_bx_destruct(&bx);
        --dequeue->size;
    }
}

void zy_dequeue_discard_last(zy_dequeue_t *dequeue)
{
    if (dequeue->size != 0)
    {
        zy_dequeue_bx_t *bx = dequeue->last;
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
        zy_dequeue_bx_destruct(&bx);
        --dequeue->size;
    }
}

const zy_opaque_t *zy_dequeue_peek_first(const zy_dequeue_t *dequeue)
{
    return &dequeue->first->opaque;
}

const zy_opaque_t *zy_dequeue_peek_last(const zy_dequeue_t *dequeue)
{
    return &dequeue->last->opaque;
}

size_t zy_dequeue_size(const zy_dequeue_t *dequeue)
{
    return dequeue->size;
}

_Bool zy_dequeue_is_empty(const zy_dequeue_t *dequeue)
{
    return dequeue->size == 0;
}