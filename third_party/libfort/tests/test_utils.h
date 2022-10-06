#ifndef TEST_UTILS_H
#define TEST_UTILS_H

struct ft_table;

int set_test_props_for_table(struct ft_table *table);
int set_test_properties_as_default(void);
struct ft_table *create_test_int_table(int set_test_opts);
struct ft_table *create_test_int_wtable(int set_test_opts);

#endif // TEST_UTILS_H
