// Capture the real module's assertions without terminating the test process.
#ifndef MCNCC_TEST_FDWATCH_ASSERT_H
#define MCNCC_TEST_FDWATCH_ASSERT_H
#ifdef __cplusplus
extern "C" {
#endif
void fw_assert_fail(const char*, const char*, int);
#ifdef __cplusplus
}
#endif
#undef assert
#define assert(expression) ((expression) ? (void)0 : fw_assert_fail(#expression, __FILE__, __LINE__))
#endif
