/** Common functionality for tests. */

#ifndef _TESTLIB_H_
#define _TESTLIB_H_

#include <string>

const std::string localhost = "localhost";
const int dispatcherPort = 8113;
const int schedulerPort = 8112;

#define ASSERT(x) status = status && (x);

/** Moving average functions */
/* sizes: tiny   - 150KB
 *        small  - 1.5MB
 *        medium - 15MB
 *        large  - 150MB
 */

const int mavg_data_150K = 384 * 100;
const int mavg_data_size = 384 * 100 * 10;
const int mavg_data_15M  = 384 * 100 * 100;
const int mavg_data_150M = 384 * 100 * 100 * 10;

int* mavg_data(int n);
int* mavg_threept_exp(int n, int* data);
bool mavg_check(int n, int* out, int* exp);

#endif /* _TESTLIB_H_ */
