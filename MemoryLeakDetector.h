#ifndef MEMORY_LEAK_DETECTOR
#define MEMORY_LEAK_DETECTOR

#include "MallocList.h"
#include "MemoryAllocation.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <exception>
#include <sstream>

class MemoryLeakDetector
{
   static MemoryLeakDetector instance;

   MallocList<MemoryAllocation> allocations;
   std::ostream& os;
   bool realTimeTracking;
   size_t totalAllocated;
   size_t totalDeleted;
   size_t maxOutstandingAllocatedBytes;
   

   MemoryLeakDetector() : 
      os(std::cout), 
      realTimeTracking(false), 
      totalAllocated(0), 
      totalDeleted(0) 
   {}

   size_t getOutstandingAllocatedBytes() { return totalAllocated - totalDeleted; }

   std::string getByteData()
   {
      std::ostringstream oss;
      oss   << "Total bytes allocated: " << totalAllocated << "\n"
            << "Total bytes deleted: " << totalDeleted << "\n"
            << "Allocated bytes not yet deleted: " << getOutstandingAllocatedBytes() << "\n";
      return oss.str();
   }

public:
   static void setTracking(bool trackingOn) { instance.realTimeTracking = trackingOn; }

   void openAllocation(void* memory, size_t size, AlloType alloType) 
   { 
      MemoryAllocation allo(memory, size, alloType);

      allocations.insert(allo);
      totalAllocated += allo.getSize();
      maxOutstandingAllocatedBytes = 
         std::max(getOutstandingAllocatedBytes(), maxOutstandingAllocatedBytes);
      
      if (realTimeTracking)
         os << "Allocating:\n" << allo << getByteData() << "\n";
   }
   
   void closeAllocation(void* memory, AlloType expectedType)
   {
      auto allo = allocations.popIf(
         [memory](auto& allo) { return allo.getLocation() == memory; }
      );

      if (expectedType != allo.getType())
      {
         std::ostringstream oss;
         oss   << "\nMemory Leak Detector Error: The following object was deallocated incorrectly:\n"
               << allo
               << "Use " << (
                  allo.getType() == SINGLE ? 
                     "'delete' instead of 'delete[]' for single object allocations" :
                     "'delete[]' instead of 'delete' for array allocations"
                  );

         throw std::runtime_error(oss.str());
      }
      totalDeleted += allo.getSize();

      if (realTimeTracking)
         os << "Deallocating:\n" << allo << getByteData() << "\n";
   }

   MemoryLeakDetector(MemoryLeakDetector&) = delete;
   ~MemoryLeakDetector() { os << *this; }
   MemoryLeakDetector& operator=(MemoryLeakDetector&) = delete;

   static MemoryLeakDetector& getInstance()
   {
      return instance;
   }

   friend std::ostream& operator<<(std::ostream& os, MemoryLeakDetector& mld)
   {
      os << mld.allocations.getLength() << " memory leak"
         << (mld.allocations.getLength() == 1 ? "" : "s") << " found:\n";

      for (auto& allo : mld.allocations)
         os << allo << "\n";
      
      return
         os << mld.getByteData() 
            << "Maximum outstanding allocated bytes at one time: " 
            << mld.maxOutstandingAllocatedBytes << "\n";
   } 
};

MemoryLeakDetector MemoryLeakDetector::instance;

void* operator new(size_t size)
{  
   if (void* ptr = std::malloc(size))
   {
      MemoryLeakDetector::getInstance().openAllocation(ptr, size, SINGLE);
      return ptr;
   }
   
   throw std::bad_alloc();
}

void* operator new[](size_t size)
{  
   if (void* ptr = std::malloc(size))
   {
      MemoryLeakDetector::getInstance().openAllocation(ptr, size, ARRAY);
      return ptr;
   }
   
   throw std::bad_alloc();
}

void operator delete(void* memory, size_t size) 
{
   MemoryLeakDetector::getInstance().closeAllocation(memory, SINGLE);
   std::free(memory);
}

void operator delete[](void* memory) 
{
   MemoryLeakDetector::getInstance().closeAllocation(memory, ARRAY);
   std::free(memory);
}

#endif