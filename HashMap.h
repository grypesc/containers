#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>

namespace Maps {

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  std::list <value_type> array[64007];
  size_type counter;
  size_type first, last;
  const size_type maxSize;
public:

  HashMap():maxSize(64007)
  {
    counter=0;
    first=0;
    last=0;
  }

  HashMap(std::initializer_list<value_type> list):HashMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      (*this)[(*it).first]=(*it).second;
  }

  HashMap(const HashMap& other):HashMap()
  {
    *this = other;
  }

  HashMap(HashMap&& other):maxSize(other.maxSize)
  {
    *this = std::move(other);
  }

  HashMap& operator=(const HashMap& other)
  {
  	if (*this==other)
  		return *this;
  	erase();
  	first=other.first;
  	last=other.last;
  	ConstIterator itr(other);
  	for (itr=other.begin();itr!=other.end();itr++)
       (*this)[(*itr).first]=(*itr).second;
  	return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(array==other.array)
      return *this;
  	counter=other.counter;
  	first=other.first;
  	last=other.last;
  	for (size_type current=0;current<maxSize;current++)
  		array[current]=std::move(other.array[current]);
  	other.counter=0;
  	return *this;
  }

  ~HashMap()
  {
	  erase();
  }

private:
  size_type hashFunction(const key_type& key) const
  {
    std::hash<key_type> hash;
    size_type hashedKey=hash(key);
    hashedKey=hashedKey%maxSize;
    return hashedKey;
  }
public:

  bool isEmpty() const
  {
    return counter==0;
  }

  mapped_type& operator[](const key_type& key)
  {
    Iterator itr(*this);
    itr = find (key);
    if (itr.it!=array[last].end())
      return (*itr).second;

    size_type hashedKey=hashFunction(key);
    value_type newOne(key, mapped_type());
    array[hashedKey].push_front(newOne);
    counter++;
     if (counter==1)
     {
       first=hashedKey;
       last=hashedKey;
     }
     else
     {
       if(hashedKey<first)
          first=hashedKey;
       if(hashedKey>last)
          last=hashedKey;
     }
     return array[hashedKey].front().second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if (counter==0)
      throw std::out_of_range ("Calling valueOf() when the map is empty");

    Iterator valueIt = find(key);
    if (valueIt.it==array[last].end())
      throw std::out_of_range ("Calling valueOf() with nonexisting key");

    return (*valueIt).second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    if (counter==0)
      throw std::out_of_range ("Calling valueOf() when the map is empty");

    Iterator valueIt = find(key);
    if (valueIt.it==array[last].end())
      throw std::out_of_range ("Calling valueOf() with nonexisting key");

    return (*valueIt).second;
  }

  const_iterator find(const key_type& key) const
  {
    size_type hashedKey=hashFunction(key);
    ConstIterator toReturn(*this);

    for (toReturn.it=array[hashedKey].begin();toReturn.it!=array[hashedKey].end();toReturn.it++)
    {
      if((*(toReturn.it)).first==key)
        {
          toReturn.current=hashedKey;
          return toReturn;
        }
    }
    toReturn.it=array[last].end();
    toReturn.current=last;
    return toReturn;
  }

  iterator find(const key_type& key)
  {
    size_type hashedKey=hashFunction(key);
    Iterator toReturn(*this);

    for (toReturn.it=array[hashedKey].begin();toReturn.it!=array[hashedKey].end();toReturn.it++)
    {
      if((*(toReturn.it)).first==key)
        {
          toReturn.current=hashedKey;
          return toReturn;
        }
    }
    toReturn.it=array[last].end();
    toReturn.current=last;
    return toReturn;
  }

  void remove(const key_type& key)
  {
    auto itr = find (key);
    if (itr.it==array[last].end())
      throw std::out_of_range ("Removal of nonexisting node");
    size_type hashedKey=hashFunction(key);
    typename std::list<value_type>::iterator a;
    for (a=array[hashedKey].begin();(*a).first!=key;a++){}

    array[hashedKey].erase(a);
  	counter--;
  	itr.it=array[hashedKey].begin();
  	if (isEmpty())
  	{
  		last=0;
  		first=0;
  		return;
  	}
  	if (hashedKey==last && array[hashedKey].empty())
  	{
  		itr--;
  		while (array[itr.current].empty())
  			itr--;
  		last=itr.current;
  		return;
  	}

  	if (hashedKey==first && array[hashedKey].empty())
  	{
  		itr++;
  		while (array[itr.current].empty())
  			itr++;
  		first=itr.current;
  		return;
  	}
  }


  void remove(const const_iterator& itr)
  {
    remove((*(itr.it)).first);
  }

  void erase()
  {
	for (iterator itr=begin();getSize()!=0;itr=begin())
		remove(itr);
  }

  size_type getSize() const
  {
    return counter;
  }

  bool operator==(const HashMap& other) const
  {
    if (counter!=other.counter)
      return 0;

    auto itThis=begin();
    for (auto itr=other.begin(); itr!=other.end();itr++)
      {
        if (*(itr.it) != *(itThis.it))
          return 0;
        itThis++;
      }
      return 1;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Iterator toReturn(*this);
    toReturn.it=array[first].begin();
    toReturn.current=first;
    return toReturn;
  }

  iterator end()
  {
    Iterator toReturn(*this);
    toReturn.it=array[last].end();
    toReturn.current=last;
    return toReturn;
  }

  const_iterator cbegin() const
  {
    ConstIterator toReturn(*this);
    toReturn.it=array[first].begin();
    toReturn.current=first;
    return toReturn;
  }

  const_iterator cend() const
  {
    ConstIterator toReturn(*this);
    toReturn.it=array[last].end();
    toReturn.current=last;
    return toReturn;
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

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  friend HashMap<KeyType, ValueType>;
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

  typename std::list<value_type>::const_iterator it;
  size_type current;
  const HashMap<KeyType, ValueType>& container;


  explicit ConstIterator( const HashMap<KeyType, ValueType>& container)
                        :it(container.array[0].begin()), current(0), container(container){}

  ConstIterator(const ConstIterator& other)
                :it(other.it), current(other.current), container(other.container){}

  ConstIterator& operator++()
  {
    if (it == container.array[container.last].end())
      throw std::out_of_range("Iterator++");

    it++;
    if (it==container.array[container.last].end())
      return *this;

    if (it==container.array[current].end())
      {
        current++;
        while(container.array[current].empty())
          current++;
        it=container.array[current].begin();
      }

    return *this;
  }

  ConstIterator operator++(int)
  {
    return ++(*this);
  }

  ConstIterator& operator--()
  {
    if (it == container.array[container.first].begin())
      throw std::out_of_range("Iterator--");

    if (it==container.array[current].begin())
      {
        current--;
        while(container.array[current].empty())
          current--;
        it=--container.array[current].end();
      }
      else
        it--;

    return *this;
  }

  ConstIterator operator--(int)
  {
    return --(*this);
  }

  reference operator*() const
  {
    if (container.counter==0 || it==container.array[current].end())
		  throw std::out_of_range ("Operator *");
    return *it;
  }

  pointer operator->() const
  {
    if (container.counter==0 || it==container.array[current].end())
		  throw std::out_of_range ("Operator ->");
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return it == other.it;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }

  ConstIterator& operator =( const ConstIterator & other )
  {
      if( & other != this )
      {
          it=other.it;
          current=other.current;
      }
      return * this;
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;


  explicit Iterator(HashMap<KeyType, ValueType>& container)
                    :ConstIterator(container)
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

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};
}
