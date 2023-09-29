
# A single-producer, single-consumer lock-free queue for C++

This mini-repository has my very own implementation of a lock-free queue (that I designed from scratch) for C++.

It only supports a two-thread use case (one consuming, and one producing). The threads can't switch roles, though
you could use this queue completely from a single thread if you wish (but that would sort of defeat the purpose!).

Note: If you need a general-purpose multi-producer, multi-consumer lock free queue, I have [one of those too][mpmc].

This repository also includes a [circular-buffer SPSC queue][circular] which supports blocking on enqueue as well as dequeue.


## Features

- [Blazing fast][benchmarks]
- Compatible with C++11 (supports moving objects instead of making copies)
- Fully generic (templated container of any type) -- just like `std::queue`, you never need to allocate memory for elements yourself
  (which saves you the hassle of writing a lock-free memory manager to hold the elements you're queueing)
- Allocates memory up front, in contiguous blocks
- Provides a `try_enqueue` method which is guaranteed never to allocate memory (the queue starts with an initial capacity)
- Also provides an `enqueue` method which can dynamically grow the size of the queue as needed
- Also provides `try_emplace`/`emplace` convenience methods
- Has a blocking version with `wait_dequeue`
- Completely "wait-free" (no compare-and-swap loop). Enqueue and dequeue are always O(1) (not counting memory allocation)
- On x86, the memory barriers compile down to no-ops, meaning enqueue and dequeue are just a simple series of loads and stores (and branches)


## Use

Simply drop the readerwriterqueue.h (or readerwritercircularbuffer.h) and atomicops.h files into your source code and include them :-)
A modern compiler is required (MSVC2010+, GCC 4.7+, ICC 13+, or any C++11 compliant compiler should work).

Note: If you're using GCC, you really do need GCC 4.7 or above -- [4.6 has a bug][gcc46bug] that prevents the atomic fence primitives
from working correctly.

Example:

```cpp
using namespace moodycamel;

ReaderWriterQueue<int> q(100);       // Reserve space for at least 100 elements up front

q.enqueue(17);                       // Will allocate memory if the queue is full
bool succeeded = q.try_enqueue(18);  // Will only succeed if the queue has an empty slot (never allocates)
assert(succeeded);

int number;
succeeded = q.try_dequeue(number);  // Returns false if the queue was empty

assert(succeeded && number == 17);

// You can also peek at the front item of the queue (consumer only)
int* front = q.peek();
assert(*front == 18);
succeeded = q.try_dequeue(number);
assert(succeeded && number == 18);
front = q.peek(); 
assert(front == nullptr);           // Returns nullptr if the queue was empty
```

The blocking version has the exact same API, with the addition of `wait_dequeue` and
`wait_dequeue_timed` methods:

```cpp
BlockingReaderWriterQueue<int> q;

std::thread reader([&]() {
    int item;
#if 1
    for (int i = 0; i != 100; ++i) {
        // Fully-blocking:
        q.wait_dequeue(item);
    }
#else
    for (int i = 0; i != 100; ) {
        // Blocking with timeout
        if (q.wait_dequeue_timed(item, std::chrono::milliseconds(5)))
            ++i;
    }
#endif
});
std::thread writer([&]() {
    for (int i = 0; i != 100; ++i) {
        q.enqueue(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
});
writer.join();
reader.join();

assert(q.size_approx() == 0);
```
    
Note that `wait_dequeue` will block indefinitely while the queue is empty; this
means care must be taken to only call `wait_dequeue` if you're sure another element
will come along eventually, or if the queue has a static lifetime. This is because
destroying the queue while a thread is waiting on it will invoke undefined behaviour.

The blocking circular buffer has a fixed number of slots, but is otherwise quite similar to
use:

```cpp
BlockingReaderWriterCircularBuffer<int> q(1024);  // pass initial capacity

q.try_enqueue(1);
int number;
q.try_dequeue(number);
assert(number == 1);

q.wait_enqueue(123);
q.wait_dequeue(number);
assert(number == 123);

q.wait_dequeue_timed(number, std::chrono::milliseconds(10));
```


## CMake
### Using targets in your project
Using this project as a part of an existing CMake project is easy.

In your CMakeLists.txt:
```
include(FetchContent)

FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY    https://github.com/cameron314/readerwriterqueue
  GIT_TAG           master
)

FetchContent_MakeAvailable(readerwriterqueue)

add_library(my_target main.cpp)
target_link_libraries(my_target PUBLIC readerwriterqueue)
```

In main.cpp:
```cpp
#include <readerwriterqueue.h>

int main()
{
    moodycamel::ReaderWriterQueue<int> q(100);
}
```

### Installing into system directories
As an alternative to including the source files in your project directly,
you can use CMake to install the library in your system's include directory:

```
mkdir build
cd build
cmake ..
make install
```

Then, you can include it from your source code:
```
#include <readerwriterqueue/readerwriterqueue.h>
```

## Disclaimers

The queue should only be used on platforms where aligned integer and pointer access is atomic; fortunately, that
includes all modern processors (e.g. x86/x86-64, ARM, and PowerPC). *Not* for use with a DEC Alpha processor (which has very weak memory ordering) :-)

Note that it's only been tested on x86(-64); if someone has access to other processors I'd love to run some tests on
anything that's not x86-based.

## More info

See the [LICENSE.md][license] file for the license (simplified BSD).

My [blog post][blog] introduces the context that led to this code, and may be of interest if you're curious
about lock-free programming.


[blog]: http://moodycamel.com/blog/2013/a-fast-lock-free-queue-for-c++
[license]: LICENSE.md
[benchmarks]: http://moodycamel.com/blog/2013/a-fast-lock-free-queue-for-c++#benchmarks
[gcc46bug]: http://stackoverflow.com/questions/16429669/stdatomic-thread-fence-has-undefined-reference
[mpmc]: https://github.com/cameron314/concurrentqueue
[circular]: readerwritercircularbuffer.h
