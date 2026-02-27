#include <check.h>
#include "test_bloch_torrey.h"

Suite *bloch_torrey_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("bloch_torrey.h test");

    tc_core = tcase_create("Core");

    /* Memory helpers */
    tcase_add_test(tc_core, test_alloc_3d_double_returns_zeroed_array);
    tcase_add_test(tc_core, test_alloc_3d_double_can_write_and_read);
    tcase_add_test(tc_core, test_alloc_3d_complexf_returns_zeroed_array);
    tcase_add_test(tc_core, test_alloc_2d_complexf_returns_zeroed_array);

    /* Solver integration */
    tcase_set_timeout(tc_core, 300);  /* simulation can take a while */
    tcase_add_test(tc_core, test_bloch_torrey_produces_nonzero_signal_for_healthy_geometry);

    suite_add_tcase(s, tc_core);

    return s;
}
