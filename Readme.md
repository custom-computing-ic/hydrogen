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

__Note__ To enable DFE implementations, the dispatcher must be started
on a machine with DFEs and Hardware MaxelerOS installed. The hardware
MaxelerOS library must be preloaded, so the startup command for the
dispatcher becomes:

```
LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./dispatcher 8111
```

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

# Bitstreams

A bitstream (.max) is a compiled dataflow design which can run on
DFEs. We store bitstreams in a separate submodule, to avoid cluttering
up the main repository.

You can get this with

```
git submodule init && git submodule update
```

The resulting directory should look like this:
```
src/bitstreams/
  DfeLib.h
  MovingAverage.h
  MovingAverage.c
  MovingAverage.max
  .....
```

DfeLib.h is a convenience header with all the functions exposed
by the various maxfiles.

Each bitstream exposes a set of functions for the Dispatcher to call
when running on a machine with access to DFEs. Compiling the CPU code
for bitstreams requires the MaxelerOS library and headers.
