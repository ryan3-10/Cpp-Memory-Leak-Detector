# C++ Memory Leak Detector

## Overview
This **C++ Memory Leak Detector** helps track memory allocations and deallocations, ensuring that all dynamically allocated memory is properly freed. It provides real-time tracking and reports memory leaks at the end of the program.

## Getting Started
To use this memory leak detector in your project, simply **include** the following header file at the top of your `main.cpp`:

```cpp
#include "MLD/MemoryLeakDetector.h"
```

This will automatically override `new`, `new[]`, `delete`, and `delete[]` operators, allowing the detector to track memory allocations. Make sure to place this include statement at the very top of your `main.cpp` file, otherwise some allocations/deallocations may be missed!

## Features
- Tracks memory allocations and deallocations.
- Detects incorrect deallocations (`delete` vs `delete[]`).
- Provides real-time tracking (optional).
- Reports outstanding allocations at program termination.

## Usage
### Enabling Real-Time Tracking
You can enable real-time tracking to log allocations and deallocations as they occur:

```cpp
MemoryLeakDetector::setTracking(true);
```

Of course, if you want to once again disable real-time tracking in the middle of your program, you can do so by calling this same method with `false` as the argument. Note that real-time tracking is disabled at the start of the program by default.

### Example
```cpp
#include "MLD/MemoryLeakDetector.h"

int main() {
    int* num = new int(42);      // Allocation tracked
    delete num;                  // Deallocation tracked
    
    int* arr = new int[10];      // Allocation tracked
    delete[] arr;                // Deallocation tracked
    
    return 0;
}
```

## Output Example
If a memory leak is detected, the program will output a report similar to:
```
1 memory leak found:
ARRAY
ID: 1
Size: 40 bytes
Starting location: 0x600003eb800

Total bytes allocated: 40
Total bytes deleted: 0
Allocated bytes not yet deleted: 40
Maximum outstanding allocated bytes at one time: 40
```

## Notes
- Ensure that all dynamically allocated memory is deallocated properly.
- An exception will be thrown if `delete` or `delete[]` are used when the other is appropriate.
- The detector prints a summary of memory leaks when the program exits.

## Author
Ryan Coutts

