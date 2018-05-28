#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <iostream>

namespace Linear
{

template <typename Type>
class LinkedList
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  class Node
  {
    Type data;
    Node(const Type& pom) : data(pom) {next=NULL; prev=NULL;};
    Node() {next=NULL; prev=NULL;};
    ~Node(){};
    Node* next;
    Node* prev;
  friend LinkedList;
  };

Node* head;
Node* guard;

public:
  LinkedList()
  {
    guard = new Node;
    head=guard;
  }

  LinkedList(std::initializer_list<Type> l):  LinkedList()//Constructs a container with a copy of each of the elements in l, in the same order.
  {
    typename std::initializer_list<Type>::iterator it;
    for(it=l.begin();it!=l.end();it++)
      this->append(*it);
  }

  LinkedList(const LinkedList& other):  LinkedList()//a copy constructor
  {
    *this=other;
  }

  LinkedList(LinkedList&& other)//a move constructor
  {
    guard = NULL;
    head=guard;
    *this=std::move(other);
  }

  ~LinkedList()
  {
      erase (this->begin(),this->end());
      delete guard;
  }

  LinkedList& operator=(const LinkedList& other)//copies all the elements from other into the container (with other preserving its contents)
  {
    if (this->head== other.head) return *this;
    erase (this->begin(),this->end());
    for(iterator it=other.begin();it!=other.end();it++)
      this->append(*it);
    return *this;
  }

  LinkedList& operator=(LinkedList&& other)//moves the elements of other into the container (other is left in an unspecified but valid state)
  {
    if (this->head== other.head) return *this;

    if (head!=guard)
      {
        erase (this->cbegin(),this->cend());
      //  delete guard; odpala jeden wiecej destruktor
      }

    head=other.head;
    guard=other.guard;

    other.guard=NULL;
    other.head=other.guard;
    return *this;
  }

  bool isEmpty() const
  {
    return head==guard;
  }

  size_type getSize() const //linear complexity
  {
    LinkedList<Type>::iterator p;
    size_type counter=0;
    for (p=this->begin();p!=this->end(); p++)
      counter++;
    return counter;
  }

  void append(const Type& item)
  {
    Node* newElement = new Node(item);
    if(head==guard)//list is empty
    {
      head=newElement;
      head->next=guard;
      guard->prev=head;
    }
    else
    {
      guard->prev->next=newElement;
      newElement->prev=guard->prev;
      guard->prev=guard->prev->next;
      newElement->next=guard;
    }
  }

  void prepend(const Type& item)
  {
    Node* newElement = new Node(item);
    if(head==guard)//list is empty
      this->append(item);
    else
    {
      newElement->next=head;
      head->prev=newElement;
      head=newElement;
    }
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if (insertPosition.current==guard)//new node will be appended if the iterator points at null
    {
       this->append(item);
       return;
    }
    if (insertPosition==begin())
    {
       this->prepend(item);
       return;
    }
    Node* newElement = new Node(item);
    newElement->next=insertPosition.current;
    newElement->prev=insertPosition.current->prev;
    insertPosition.current->prev->next=newElement;
    insertPosition.current->prev=newElement;
  }

  Type popFirst()
  {
    if (this->isEmpty()==1) throw std::out_of_range("An attempt to pop first element from empty list was made");
    Type itemToReturn=head->data;
    if(head->next==guard)
    {
      delete head;
      head=guard;
      return itemToReturn;
    }
    head=head->next;
    delete head->prev;
    head->prev=NULL;
    return itemToReturn;
  }

  Type popLast()
  {
    if (this->isEmpty()==1) throw std::out_of_range("An attempt to pop last element from empty list was made");
    Type itemToReturn=guard->prev->data;
    if(guard->prev==head) return this->popFirst();
    Node* toDelete=guard->prev;
    toDelete->prev->next=guard;
    guard->prev=toDelete->prev;
    delete toDelete;
    return itemToReturn;
  }

  void erase(const const_iterator& position)
  {
    if(position.current==guard)  throw std::out_of_range("An attempt to erase data at guard adress");
    if(position.current->prev==NULL)//Erasing first element. Can't pop becuase pop starts a constructor.
    {
      //popFirst(); To pass the tests it cannot be that simple :(
      if(head->next==guard)
      {
        delete head;
        head=guard;
        return;
      }
      head=head->next;
      delete head->prev;
      head->prev=NULL;
      return;
    }

    if(position.current->next==guard)//Erasing last element. Can't pop becuase pop starts a constructor.
    {
      if(guard->prev==head)
      {
        delete head;
        head=guard;
        return;
      }
      Node* toDelete=guard->prev;
      toDelete->prev->next=guard;
      guard->prev=toDelete->prev;
      delete toDelete;
      return;
    }

    position.current->next->prev=position.current->prev;
    position.current->prev->next=position.current->next;
    delete position.current;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    for(Iterator it=firstIncluded;it!=lastExcluded;it++)
      erase (it);
  }

  iterator begin()
  {
    LinkedList::Iterator it;
    it.current=this->head;
    return it;
  }

  iterator end()
  {
    LinkedList::Iterator it;
    it.current=guard;
    return it;
  }

  const_iterator cbegin() const
  {
    LinkedList::ConstIterator it;
    it.current=this->head;
    return it;
  }

  const_iterator cend() const
  {
    LinkedList::ConstIterator it;
    it.current=guard;
    return it;
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

///////////////////////////////////////////////////////////////////////////////

template <typename Type>
class LinkedList<Type>::ConstIterator
{
  friend LinkedList<Type>;
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;

private:
  Node* current;
public:

  explicit ConstIterator()
  {
    current=NULL;
  }

  reference operator*() const
  {
    if(current->next==NULL)//iterator points at the guard
      throw std::out_of_range("Pointer points on the guard");
    return current->data;
  }

  ConstIterator& operator++()
  {
    if (current->next==NULL)//iterator points at the guard
      throw std::out_of_range("Cannot increase iterator");
    current=current->next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    if (current->next==NULL)
      throw std::out_of_range("Cannot increase iterator");
    current=current->next;
    return *this;
  }

  ConstIterator& operator--()
  {
    if (current->prev==NULL)
        throw std::out_of_range("Cannot decrease iterator");
    current=current->prev;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if (current->prev==NULL)
        throw std::out_of_range("Cannot decrease iterator");
    current=current->prev;
    return *this;
  }

  ConstIterator operator+(difference_type d) const
  {
    ConstIterator itToBeReturned;
    itToBeReturned.current=this->current;
    for(difference_type i=0;i<d;i++)
        itToBeReturned++;
    return itToBeReturned;
  }

  ConstIterator operator-(difference_type d) const
  {
    ConstIterator itToBeReturned;
    itToBeReturned.current=this->current;
    for(difference_type i=0;i<d;i++)
        itToBeReturned--;
    return itToBeReturned;
  }

  bool operator==(const ConstIterator& other) const
  {
    return current==other.current;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return current!=other.current;
  }
};
/////////////////////////////////////////////////////////////////////////

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_LINKEDLIST_H
