#include "vector.h"
#include <assert.h>
#include <string.h>

struct f_vector {
    size_t m_size;
    void  *m_data;
    size_t m_capacity;
    size_t m_item_size;
};


static int vector_reallocate_(f_vector_t *vector, size_t new_capacity)
{
    assert(vector);
    assert(new_capacity > vector->m_capacity);

    size_t new_size = new_capacity * vector->m_item_size;
    vector->m_data = F_REALLOC(vector->m_data, new_size);
    if (vector->m_data == NULL)
        return -1;
    return 0;
}


FT_INTERNAL
f_vector_t *create_vector(size_t item_size, size_t capacity)
{
    f_vector_t *vector = (f_vector_t *)F_MALLOC(sizeof(f_vector_t));
    if (vector == NULL) {
        return NULL;
    }

    size_t init_size = MAX(item_size * capacity, 1);
    vector->m_data = F_MALLOC(init_size);
    if (vector->m_data == NULL) {
        F_FREE(vector);
        return NULL;
    }

    vector->m_size      = 0;
    vector->m_capacity  = capacity;
    vector->m_item_size = item_size;

    return vector;
}


FT_INTERNAL
void destroy_vector(f_vector_t *vector)
{
    assert(vector);
    F_FREE(vector->m_data);
    F_FREE(vector);
}


FT_INTERNAL
size_t vector_size(const f_vector_t *vector)
{
    assert(vector);
    return vector->m_size;
}


FT_INTERNAL
size_t vector_capacity(const f_vector_t *vector)
{
    assert(vector);
    return vector->m_capacity;
}


FT_INTERNAL
int vector_push(f_vector_t *vector, const void *item)
{
    assert(vector);
    assert(item);

    if (vector->m_size == vector->m_capacity) {
        if (vector_reallocate_(vector, vector->m_capacity * 2) == -1)
            return FT_GEN_ERROR;
        vector->m_capacity = vector->m_capacity * 2;
    }

    size_t offset = vector->m_size * vector->m_item_size;
    memcpy((char *)vector->m_data + offset, item, vector->m_item_size);

    ++(vector->m_size);

    return FT_SUCCESS;
}

FT_INTERNAL
int vector_insert(f_vector_t *vector, const void *item, size_t pos)
{
    assert(vector);
    assert(item);
    size_t needed_capacity = MAX(pos + 1, vector->m_size + 1);
    if (vector->m_capacity < needed_capacity) {
        if (vector_reallocate_(vector, needed_capacity) == -1)
            return FT_GEN_ERROR;
        vector->m_capacity = needed_capacity;
    }
    size_t offset = pos * vector->m_item_size;
    if (pos >= vector->m_size) {
        /* Data in the middle are not initialized */
        memcpy((char *)vector->m_data + offset, item, vector->m_item_size);
        vector->m_size = pos + 1;
        return FT_SUCCESS;
    } else {
        /* Shift following data by one position */
        memmove((char *)vector->m_data + offset + vector->m_item_size,
                (char *)vector->m_data + offset,
                vector->m_item_size * (vector->m_size - pos));
        memcpy((char *)vector->m_data + offset, item, vector->m_item_size);
        ++(vector->m_size);
        return FT_SUCCESS;
    }
}

FT_INTERNAL
f_vector_t *vector_split(f_vector_t *vector, size_t pos)
{
    size_t trailing_sz = vector->m_size > pos ? vector->m_size - pos : 0;
    f_vector_t *new_vector = create_vector(vector->m_item_size, trailing_sz);
    if (!new_vector)
        return new_vector;
    if (new_vector->m_capacity < trailing_sz) {
        destroy_vector(new_vector);
        return NULL;
    }

    if (trailing_sz == 0)
        return new_vector;

    size_t offset = vector->m_item_size * pos;
    memcpy(new_vector->m_data, (char *)vector->m_data + offset,
           trailing_sz * vector->m_item_size);
    new_vector->m_size = trailing_sz;
    vector->m_size = pos;
    return new_vector;
}

FT_INTERNAL
const void *vector_at_c(const f_vector_t *vector, size_t index)
{
    if (index >= vector->m_size)
        return NULL;

    return (char *)vector->m_data + index * vector->m_item_size;
}


FT_INTERNAL
void *vector_at(f_vector_t *vector, size_t index)
{
    if (index >= vector->m_size)
        return NULL;

    return (char *)vector->m_data + index * vector->m_item_size;
}


FT_INTERNAL
f_status vector_swap(f_vector_t *cur_vec, f_vector_t *mv_vec, size_t pos)
{
    assert(cur_vec);
    assert(mv_vec);
    assert(cur_vec != mv_vec);
    assert(cur_vec->m_item_size == mv_vec->m_item_size);

    size_t cur_sz = vector_size(cur_vec);
    size_t mv_sz = vector_size(mv_vec);
    if (mv_sz == 0) {
        return FT_SUCCESS;
    }

    size_t min_targ_size = pos + mv_sz;
    if (vector_capacity(cur_vec) < min_targ_size) {
        if (vector_reallocate_(cur_vec, min_targ_size) == -1)
            return FT_GEN_ERROR;
        cur_vec->m_capacity = min_targ_size;
    }

    size_t offset = pos * cur_vec->m_item_size;
    void *tmp = NULL;
    size_t new_mv_sz = 0;
    if (cur_sz > pos) {
        new_mv_sz = MIN(cur_sz - pos, mv_sz);
        tmp = F_MALLOC(cur_vec->m_item_size * new_mv_sz);
        if (tmp == NULL) {
            return FT_MEMORY_ERROR;
        }
    }

    if (tmp) {
        memcpy(tmp,
               (char *)cur_vec->m_data + offset,
               cur_vec->m_item_size * new_mv_sz);
    }

    memcpy((char *)cur_vec->m_data + offset,
           mv_vec->m_data,
           cur_vec->m_item_size * mv_sz);

    if (tmp) {
        memcpy(mv_vec->m_data,
               tmp,
               cur_vec->m_item_size * new_mv_sz);
    }

    cur_vec->m_size = MAX(cur_vec->m_size, min_targ_size);
    mv_vec->m_size = new_mv_sz;
    F_FREE(tmp);
    return FT_SUCCESS;
}

FT_INTERNAL
void vector_clear(f_vector_t *vector)
{
    vector->m_size = 0;
}

FT_INTERNAL
int vector_erase(f_vector_t *vector, size_t index)
{
    assert(vector);

    if (vector->m_size == 0 || index >= vector->m_size)
        return FT_GEN_ERROR;

    memmove((char *)vector->m_data + vector->m_item_size * index,
            (char *)vector->m_data + vector->m_item_size * (index + 1),
            (vector->m_size - 1 - index) * vector->m_item_size);
    vector->m_size--;
    return FT_SUCCESS;
}

#ifdef FT_TEST_BUILD

f_vector_t *copy_vector(f_vector_t *v)
{
    if (v == NULL)
        return NULL;

    f_vector_t *new_vector = create_vector(v->m_item_size, v->m_capacity);
    if (new_vector == NULL)
        return NULL;

    memcpy(new_vector->m_data, v->m_data, v->m_item_size * v->m_size);
    new_vector->m_size      = v->m_size ;
    new_vector->m_item_size = v->m_item_size ;
    return new_vector;
}

size_t vector_index_of(const f_vector_t *vector, const void *item)
{
    assert(vector);
    assert(item);

    size_t i = 0;
    for (i = 0; i < vector->m_size; ++i) {
        void *data_pos = (char *)vector->m_data + i * vector->m_item_size;
        if (memcmp(data_pos, item, vector->m_item_size) == 0) {
            return i;
        }
    }
    return INVALID_VEC_INDEX;
}

#endif
