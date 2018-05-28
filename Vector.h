#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace Linear
{

template <typename Type>
class Vector
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
  size_type maxSize;
  size_type currentSize;
  Type* array;
public:

  Vector()
  {
    maxSize=4;
    currentSize=0;
    array=new Type[maxSize];
  }

Vector(std::initializer_list<Type> l): Vector()
  {
    typename std::initializer_list<Type>::iterator it;
    for(it=l.begin();it!=l.end();it++)
      this->append(*it);
  }

  Vector(const Vector& other)
  {
    maxSize=4;
    currentSize=0;
    array=NULL;
    *this=other;
  }

  Vector(Vector&& other)
  {
    maxSize=4;
    currentSize=0;
    array=NULL;
    *this=std::move(other);
  }

  ~Vector()
  {
    delete[] array;
  }

  Vector& operator=(const Vector& other)
  {
    if (other.array==array)
      return *this;
    if (array!=NULL)
      delete[] array;
    maxSize=other.maxSize;
    currentSize=0;
    array=new Type[maxSize];
    ConstIterator it (other.array, other);
    for (it=other.begin();it!=other.end();it++)
      append(*it);
    return *this;
  }

  Vector& operator=(Vector&& other)
  {
    if (other.array==array)
      return *this;
    delete[] array;
    maxSize=other.maxSize;
    currentSize=other.currentSize;
    array=other.array;

    other.maxSize=4;
    other.currentSize=0;
    other.array=NULL;
    return *this;
  }

private:
  void reSize()
  {
    Type* oldArrayBegin= array;
    Type* oldArrayEnd= (array+maxSize);
    array=new Type[2*maxSize];
    this->maxSize=2*maxSize;
    currentSize=0;
    for ( Type* current=oldArrayBegin;current!=oldArrayEnd;current++)
      append(*current);
    delete[] oldArrayBegin;
  }

public:

  bool isEmpty() const
  {
    return currentSize==0;
  }

  size_type getSize() const
  {
    return currentSize;
  }

  void append(const Type& item)
  {
    insert(end(), item);
  }

  void prepend(const Type& item)
  {
    insert(begin(),item);
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    int insertPositionInt=0;
    while (array+insertPositionInt!=insertPosition.current)
      insertPositionInt++;

    if (currentSize>=maxSize) reSize();
    int position=currentSize;
    while (position!=insertPositionInt)
      {
        array[position]=array[position-1];
        position--;
      }
      array[position]=item;
      currentSize++;
  }

  Type popFirst()
  {
    if (isEmpty()) throw std::logic_error("Colection is empty. Cannot pop first element.");
    Type toReturn=array[0];
    erase(begin(),++begin());
    return toReturn;
  }

  Type popLast()
  {
    if (isEmpty()) throw std::logic_error("Colection is empty. Cannot pop last element.ss");
    currentSize--;
    return array[currentSize];
  }

  void erase(const const_iterator& position)
  {
    const_iterator it=position;
    erase(position, ++it);
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    size_type toWrite=0;
    size_type from=0;

    while (array+toWrite!=firstIncluded.current)
      toWrite++;
    while (array+from!=lastExcluded.current)
      from++;
    size_type numberOfErasedElements=from-toWrite;

    while (from<currentSize)
    {
      array[toWrite]=array[from];
      toWrite++;
      from++;
    }
    currentSize-=numberOfErasedElements;
  }

  iterator begin()
  {
    return iterator (array, *this);
  }

  iterator end()//iterator points at element following the last element
  {
    return iterator (array+currentSize, *this);
  }

  const_iterator cbegin() const
  {
    return const_iterator (array, *this);
  }

  const_iterator cend() const
  {
    return const_iterator (array+currentSize, *this);

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
////////////////////////////////////////////////////////////////////////////////
template <typename Type>
class Vector<Type>::ConstIterator
{
  friend Vector<Type>;
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

private:
  Type* current;
  const Vector<Type>& container;

public:
  explicit ConstIterator(Type* current,const Vector<Type>& container)
                        : current(current), container(container){}

  explicit ConstIterator(const Vector<Type>& container)
                        : container(container){}

  reference operator*() const
  {
    if (*this==container.end())
      throw std::out_of_range("Iterator out of range");
    return *current;
  }

  ConstIterator& operator++()
  {
    if (*this==container.end())
      throw std::out_of_range("Iterator out of range");
    this->current++;
    return *this;
  }

  ConstIterator operator++(int)
  {
    if (*this==container.end())
      throw std::out_of_range("Iterator out of range");
    this->current++;
    return *this;
  }

  ConstIterator& operator--()
  {
    if (*this==container.begin())
      throw std::out_of_range("Iterator out of range");
    this->current--;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if (*this==container.begin())
      throw std::out_of_range("Iterator out of range");
    this->current--;
    return *this;
  }

  ConstIterator operator+(difference_type d) const
  {
    ConstIterator toReturn (this->current,this->container);
    for(difference_type i=1; i<=d; i++)
      toReturn++;
    return toReturn;
  }

  ConstIterator operator-(difference_type d) const
  {
    ConstIterator toReturn (this->current,this->container);
    for(difference_type i=1; i<=d; i++)
      toReturn--;
    return toReturn;
  }

  bool operator==(const ConstIterator& other) const
  {
    return current==other.current;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return this->current!=other.current;
  }

  ConstIterator& operator=(const ConstIterator& other) // copy assignment
  {
    this->current=other.current;
    return *this;
  }
};
/////////////////////////////////////////////////////////////////////////////////
template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;

  explicit Iterator(Type* current, Vector<Type>& container)
  : ConstIterator(current, container){}

  Iterator(const ConstIterator& other)
  : ConstIterator(other){}

  explicit Iterator( const Vector<Type>& container)
  : ConstIterator(container){}

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

