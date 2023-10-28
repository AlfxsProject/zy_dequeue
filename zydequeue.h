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

typedef struct zydequeue_s zydequeue_t;

typedef struct opaque_s
{
    const size_t size;
    const unsigned char data[0];
} opaque_t;

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((nonnull)) int zydequeue_construct(zydequeue_t **zydequeue, const zyalloc_t *alloc);
    __attribute__((nonnull)) void zydequeue_destruct(zydequeue_t **zydequeue);
    __attribute__((nonnull)) void zydequeue_clear(zydequeue_t *zydequeue);
    __attribute__((nonnull)) int zydequeue_push_first(zydequeue_t *zydequeue, const opaque_t *opaque);
    __attribute__((nonnull)) int zydequeue_push_last(zydequeue_t *zydequeue, const opaque_t *opaque);
    __attribute__((nonnull)) void zydequeue_discard_first(zydequeue_t *zydequeue);
    __attribute__((nonnull)) void zydequeue_discard_last(zydequeue_t *zydequeue);
    __attribute__((nonnull)) const opaque_t *zydequeue_peek_first(const zydequeue_t *zydequeue);
    __attribute__((nonnull)) const opaque_t *zydequeue_peek_last(const zydequeue_t *zydequeue);
    __attribute__((nonnull)) size_t zydequeue_size(const zydequeue_t *zydequeue);
    __attribute__((nonnull)) _Bool zydequeue_is_empty(const zydequeue_t *zydequeue);

#ifdef __cplusplus
}
#endif