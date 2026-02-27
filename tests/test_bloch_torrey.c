#include <check.h>
#include "test_bloch_torrey.h"

Suite *bloch_torrey_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_slow;

    s = suite_create("bloch_torrey.h test");

    tc_core = tcase_create("Core");

    /* Memory helpers */
    tcase_add_test(tc_core, test_alloc_3d_double_returns_zeroed_array);
    tcase_add_test(tc_core, test_alloc_3d_double_can_write_and_read);
    tcase_add_test(tc_core, test_alloc_3d_complexf_returns_zeroed_array);
    tcase_add_test(tc_core, test_alloc_2d_complexf_returns_zeroed_array);

    /* Solver integration */
    tcase_add_test(tc_core, test_bloch_torrey_produces_nonzero_signal_for_isotropic_cube);
    tcase_add_test(tc_core, test_bloch_torrey_signal_constant_without_gradient);

    suite_add_tcase(s, tc_core);

    /* Regression tests (slow: use full-resolution geometry, ~20 s each) */
    tc_slow = tcase_create("Slow");
    tcase_set_timeout(tc_slow, 60);
    tcase_add_test(tc_slow, test_bloch_torrey_2011_normalised_decay_matches_reference);
    suite_add_tcase(s, tc_slow);

    return s;
}
