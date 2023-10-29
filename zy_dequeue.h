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
#pragma once
#include <zyalloc.h>

typedef struct zy_dequeue_s zy_dequeue_t;

#ifndef ZY_OPAQUE_TYPE
#define ZY_OPAQUE_TYPE
typedef struct opaque_s
{
    const size_t size;
    const unsigned char data[0];
} zy_opaque_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((nonnull)) int zy_dequeue_construct(zy_dequeue_t **dequeue, const zyalloc_t *alloc);
    __attribute__((nonnull)) void zy_dequeue_destruct(zy_dequeue_t **dequeue);
    __attribute__((nonnull)) void zy_dequeue_clear(zy_dequeue_t *dequeue);
    __attribute__((nonnull)) int zy_dequeue_push_first(zy_dequeue_t *dequeue, const zy_opaque_t *opaque);
    __attribute__((nonnull)) int zy_dequeue_push_last(zy_dequeue_t *dequeue, const zy_opaque_t *opaque);
    __attribute__((nonnull)) void zy_dequeue_discard_first(zy_dequeue_t *dequeue);
    __attribute__((nonnull)) void zy_dequeue_discard_last(zy_dequeue_t *dequeue);
    __attribute__((nonnull)) const zy_opaque_t *zy_dequeue_peek_first(const zy_dequeue_t *dequeue);
    __attribute__((nonnull)) const zy_opaque_t *zy_dequeue_peek_last(const zy_dequeue_t *dequeue);
    __attribute__((nonnull)) size_t zy_dequeue_size(const zy_dequeue_t *dequeue);
    __attribute__((nonnull)) _Bool zy_dequeue_is_empty(const zy_dequeue_t *dequeue);

#ifdef __cplusplus
}
#endif