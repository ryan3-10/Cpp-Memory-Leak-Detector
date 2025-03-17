#ifndef MEMORY_ALLOCATION
#define MEMORY_ALLOCATION

#include <iostream>
enum AlloType {SINGLE, ARRAY};

class MemoryAllocation
{
   static inline int count = 0;
   const void* memoryLocation;
   const size_t size;
   const AlloType type;
   const int id;

public:
   MemoryAllocation(void* memoryLocation, size_t size, AlloType type) : 
      memoryLocation(memoryLocation), size(size), type(type), id(++count) {}

   AlloType getType() const { return type; }
   size_t getSize() const { return size; }
   const void* getLocation() const { return memoryLocation; }

   friend std::ostream& operator<<(std::ostream& os, MemoryAllocation& allo)
   {
      if (allo.type == SINGLE) os << "SINGLE OBJECT\n";
      else os << "ARRAY\n";

      return 
         os << "ID: " << allo.id << "\n"
            << "Size: " << allo.size << " bytes\n"
            << "Starting location: "<< allo.memoryLocation << "\n";    
   }
};

#endif