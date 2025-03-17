#ifndef MALLOC_LIST
#define MALLOC_LIST

#include <iostream>
#include <functional>
#include <exception>

template <typename T>
class MallocList
{
   struct Node
   {
      T data;
      Node* next;
      Node(T data) : data(data), next(nullptr) {}
      ~Node() = default;
   };

   class Iterator
   {
      Node* current;

   public:
      Iterator(Node* node) : current(node) {}

      Iterator& operator++()
      {
         current = current->next;
         return *this;
      }

      Iterator operator++(int)
      {
         auto temp = *this;
         current = current->next;
         return temp;
      }

      T& operator*() { return current->data; }
      bool operator==(Iterator other) { return current == other.current; }
      bool operator!=(Iterator other) { return current != other.current; }
   };

   Node* head;
   size_t length;

public:
   MallocList() : head(nullptr), length(0) {}

   ~MallocList()
   {
      auto current = head;
      Node* next;

      while(current)
      {
         next = current->next;
         current->~Node();
         std::free(current);
         current = next;
      }
   }

   Iterator begin() const { return Iterator(head); }
   Iterator end() const { return Iterator(nullptr); }

   size_t getLength() const { return length; }

   T popIf(std::function<bool(T&)> condition) 
   {
      if (!head)
         throw std::runtime_error("popIf called on empty list");

      if (condition(head->data))
      {
         auto result = head->data;
         auto newHead = head->next;
         std::free(head);
         head = newHead;
         length--;
         return result;
      }

      auto current = head;

      while (current->next)
      {
         if (condition(current->next->data))
         {
            auto result = current->next->data;
            auto newNext = current->next->next;
            std::free(current->next);
            current->next = newNext;
            length--;
            return result;
         }
         
         current = current->next;
      }

      throw std::runtime_error("popIf condition was not true for any elements in list");
   }

   void insert(T data)
   {
      void* ptr = std::malloc(sizeof(Node));
      if (!ptr) throw std::bad_alloc();

      auto newNode = new(ptr) Node(data);
      if (length++ == 0) head = newNode;
      else
      {
         newNode->next = head;
         head = newNode;
      }
   }
};

#endif