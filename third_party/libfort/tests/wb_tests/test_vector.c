#include "tests.h"
#include "vector.h"


void test_vector_basic(void)
{
    size_t i = 0;

    typedef short item_t;
    const size_t init_capacity = 10;
    f_vector_t *vector = create_vector(sizeof(item_t), init_capacity);

    assert_true(vector != NULL);
    assert_true(vector_size(vector)     == 0);
    assert_true(vector_capacity(vector) == init_capacity);

    WHEN("Pushing less items than initial capacity") {
        for (i = 0; i < init_capacity; ++i) {
            item_t item = (item_t)i;
            vector_push(vector, &item);
        }

        THEN("Then capacity is not changed") {
            assert_true(vector_size(vector)     == init_capacity);
            assert_true(vector_capacity(vector) == init_capacity);
        }
    }

    WHEN("Clearing vector") {
        vector_clear(vector);

        THEN("Then size == 0") {
            assert_true(vector_size(vector) == 0);
        }
    }

    WHEN("Pushing more items than initial capacity") {
        for (i = 0; i < 2 * init_capacity; ++i) {
            item_t item = (item_t)(2 * i);
            vector_push(vector, &item);
        }

        THEN("Then capacity is increased") {
            assert_true(vector_size(vector)     == 2 * init_capacity);
            assert_true(vector_capacity(vector)  > init_capacity);
        }

#ifdef FT_TEST_BUILD
        WHEN("Checking indexes of items") {
            item_t item =  6;
            assert_true(vector_index_of(vector, &item) == 3);
            item =  14;
            assert_true(vector_index_of(vector, &item) == 7);
            item =  25;
            assert_true(vector_index_of(vector, &item) == INVALID_VEC_INDEX);
        }
#endif

        WHEN("Checking access to items") {
            assert_true(*(item_t *)vector_at(vector,  0) == 0);
            assert_true(*(item_t *)vector_at(vector, 10) == 20);
            assert_true((item_t *)vector_at(vector, 20) == NULL);
        }

#ifdef FT_TEST_BUILD
        WHEN("Erasing items") {
            assert_true(vector_erase(vector, 20) != FT_SUCCESS);

            assert_true(vector_erase(vector,  0) == FT_SUCCESS);
            assert_true(vector_erase(vector, 10) == FT_SUCCESS);

            item_t first_item = *(item_t *)vector_at(vector, 0);
            assert_true(first_item == 2);
            item_t item =  6;
            assert_true(vector_index_of(vector, &item) == 2);
            item =  26;
            assert_true(vector_index_of(vector, &item) == 11);
        }
#endif
    }

    WHEN("Clearing vector") {
        vector_clear(vector);

        THEN("Then size == 0") {
            assert_true(vector_size(vector) == 0);
        }
    }

    WHEN("Testing insert method vector") {
        vector_clear(vector);

        size_t capacity = 10 * init_capacity;
        for (i = 0; i < capacity; ++i) {
            item_t item = (item_t)i;
            vector_insert(vector, &item, 0);
        }
        assert_true(vector_size(vector) == capacity);
        for (i = 0; i < capacity; ++i) {
            assert_true(*(item_t *)vector_at(vector, i) == (item_t)(capacity - i) - 1);
        }

        item_t item_666 = 666;
        vector_insert(vector, &item_666, 5 * capacity - 1);
        assert_true(vector_size(vector) == 5 * capacity);
        assert_true(*(item_t *)vector_at(vector, 5 * capacity - 1) == item_666);

        item_t item_777 = 777;
        vector_insert(vector, &item_777, 10);
        assert_true(vector_size(vector) == 5 * capacity + 1);
        assert_true(*(item_t *)vector_at(vector, 10) == item_777);
    }

    WHEN("Moving from another vector") {
        vector_clear(vector);
        for (i = 0; i < 10; ++i) {
            item_t item = (item_t)i;
            vector_push(vector, &item);
        }

        f_vector_t *mv_vector = create_vector(sizeof(item_t), 5);
        assert_true(mv_vector != NULL);
        for (i = 0; i < 5; ++i) {
            item_t item = (item_t)i * 2;
            assert_true(vector_push(mv_vector, &item) ==  FT_SUCCESS);
        }
        assert_true(vector_swap(vector, mv_vector, 2) == FT_SUCCESS);
        destroy_vector(mv_vector);
        assert_true(vector_size(vector) == 10);

        assert_true(*(item_t *)vector_at(vector, 1) == 1); /* original value */
        assert_true(*(item_t *)vector_at(vector, 2) == 0); /* inserted value */
        assert_true(*(item_t *)vector_at(vector, 4) == 4); /* inserted value */
        assert_true(*(item_t *)vector_at(vector, 9) == 9); /* original value */

        mv_vector = create_vector(sizeof(item_t), 5);
        assert_true(mv_vector != NULL);
        for (i = 0; i < 5; ++i) {
            item_t item = (item_t)i * 2;
            assert_true(vector_push(mv_vector, &item) ==  FT_SUCCESS);
        }
        assert_true(vector_swap(vector, mv_vector, 10) == FT_SUCCESS);
        destroy_vector(mv_vector);
        assert_true(vector_size(vector) == 15);
    }

    destroy_vector(vector);
}


void test_vector_stress(void)
{
    size_t i = 0;

    typedef short item_t;
    const size_t init_capacity = 10;
    f_vector_t *vector = create_vector(sizeof(item_t), init_capacity);

    assert_true(vector != NULL);
    assert_true(vector_size(vector)     == 0);
    assert_true(vector_capacity(vector) == init_capacity);

    WHEN("Pushing a lot of items into vector") {
        for (i = 0; i < 1000 * init_capacity; ++i) {
            item_t item = (item_t)i;
            vector_push(vector, &item);
        }

        THEN("Then capacity is not changed") {
            assert_true(vector_size(vector)     == 1000 * init_capacity);
            assert_true(vector_capacity(vector) >= 1000 * init_capacity);
        }
    }

    /*
     * Destroy without previously called clear
     */
    destroy_vector(vector);
}
