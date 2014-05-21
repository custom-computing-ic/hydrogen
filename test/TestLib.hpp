/** Common functionality for tests. */

#ifndef _TESTLIB_H_
#define _TESTLIB_H_

/** Moving average functions */
const int mavg_data_size = 16;

int* mavg_data(int n);
int* mavg_threept_exp(int n, int* data);
bool mavg_check(int n, int* out, int* exp);

#endif /* _TESTLIB_H_ */
