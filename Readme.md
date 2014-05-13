dfe-cloud
========

A cloud for heterogeneous architectures:

1. Scheduler - a job scheduler, ```src/scheduler```
2. Dispatcher - a job executor, ```src/dispatcher```
3. The client API - available in ```src/client/```.

The startup action sequence is:

1. start dispatcher (```./build/dispatcher <dispPort>```)
2. start scheduler (```./build/scheduler <schedPort> <dispPort>```)
3. run a client application (which submits jobs to the scheduler,
e.g. ```build/MovingAverageOneDfe)

# Build

Run cmake, which will create a Makefile:

```
mkdir build && cd build && cmake ..
```

Then use makefile targets:
```
make
make test
```
__Note__ You will want to start the servers locally before running
```make test``.

# Sample Program

From the user perspective, a sample program will look like:

```
#include <client_api.hpp>

int main() {
  int data[] = {1, 2, 3, 4};
  movingAverage(4, data);
}
```

This requests a moving average task to be performed.
The sequence of actions would then be:

1. client API (on behalf of user program) creates a client to connect to the scheduler
2. client API sends job description and data to the scheduler
3. scheduler sends job description to the dispatcher
4. dispatcher executes job and sends result to scheduler
5. scheduler sends result to client
6. client closes scheduler connection
