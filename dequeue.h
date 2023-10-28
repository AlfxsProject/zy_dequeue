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

typedef struct dequeue_s dequeue_t;

typedef struct opaque_s
{
    const size_t size;
    const unsigned char data[0];
} opaque_t;

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((nonnull)) int dequeue_construct(dequeue_t **dequeue, const zyalloc_t *alloc);
    __attribute__((nonnull)) void dequeue_destruct(dequeue_t **dequeue);
    __attribute__((nonnull)) void dequeue_clear(dequeue_t *dequeue);
    __attribute__((nonnull)) int dequeue_push_first(dequeue_t *dequeue, const opaque_t *opaque);
    __attribute__((nonnull)) int dequeue_push_last(dequeue_t *dequeue, const opaque_t *opaque);
    __attribute__((nonnull)) void dequeue_discard_first(dequeue_t *dequeue);
    __attribute__((nonnull)) void dequeue_discard_last(dequeue_t *dequeue);
    __attribute__((nonnull)) const opaque_t *dequeue_peek_first(const dequeue_t *dequeue);
    __attribute__((nonnull)) const opaque_t *dequeue_peek_last(const dequeue_t *dequeue);
    __attribute__((nonnull)) size_t dequeue_size(const dequeue_t *dequeue);
    __attribute__((nonnull)) _Bool dequeue_is_empty(const dequeue_t *dequeue);

#ifdef __cplusplus
}
#endif