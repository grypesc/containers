#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace Maps {

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair< key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;
  enum Color {RED, BLACK};
private:

  class Node
  {
    value_type data;
    Color color;
    Node* left;
    Node* right;
    Node* parent;
    Node() {right=NULL; left=NULL; parent=NULL;};
    ~Node(){};
    friend TreeMap;
  };

  Node* root;
  Node* guard;
  size_type counter;
public:

  TreeMap()
  {
    guard= new Node;
    guard->color=BLACK;
    root=guard;
    guard->parent=guard;
    counter=0;
  }

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it)
      (*this)[(*it).first]=(*it).second;
  }

  TreeMap(const TreeMap& other): TreeMap()
  {
    *this = other;
  }

  TreeMap(TreeMap&& other)
  {
    root=NULL;
    guard=root;
    *this = std::move(other);
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if (root != other.root)
      erase();
    for (auto it=other.begin(); it!=other.end();it++)
      (*this)[(*it).first]=(*it).second;
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if (root != other.root)
      erase();
    root=other.root;
    guard=other.guard;
    counter=other.counter;

    other.root=NULL;
    other.guard=NULL;
    other.counter=0;
    return *this;
  }

  ~TreeMap()
  {
    erase();
  }

////////////////////////////////////////////////////////////////////////////////
private:
  void leftRotate (Node* x)
  {
    if (x->right==guard)
      return;
    Node* y = x->right;
    x->right=y->left;
    if (y->left!=guard)
      y->left->parent=x;
    y->parent=x->parent;
    if(x->parent==guard)
      root=y;
    else
    {
      if(x==x->parent->left)
        x->parent->left=y;
      else
        x->parent->right=y;
    }
    y->left=x;
    x->parent=y;
  }

  void rightRotate (Node* y)
  {
    if (y->left==guard)
      return;
    Node* x = y->left;
    y->left=x->right;
    if (x->right!=guard)
      x->right->parent=y;
    x->parent=y->parent;
    if(y->parent==guard)
      root=x;
    else
    {
      if(y==y->parent->left)
        y->parent->left=x;
        else
        y->parent->right=x;
    }
    x->right=y;
    y->parent=x;
  }

  void insertFixUp(Node* z)
  {
    Node* y;
    while (z->parent->color==RED)
    {
      if (z->parent==z->parent->parent->left)
      {
        y=z->parent->parent->right;
        if(y->color==RED)
        {
          z->parent->color=BLACK;
          y->color=BLACK;
          z->parent->parent->color=RED;
          z=z->parent->parent;
        }
        else
        {
          if (z==z->parent->right)
          {
            z=z->parent;
            leftRotate(z);
          }
          z->parent->color=BLACK;
          z->parent->parent->color=RED;
          rightRotate(z->parent->parent);
        }
      }
      else
      {
        y=z->parent->parent->left;
        if(y->color==RED)
        {
          z->parent->color=BLACK;
          y->color=BLACK;
          z->parent->parent->color=RED;
          z=z->parent->parent;
        }
        else
        {
          if (z==z->parent->left)
          {
            z=z->parent;
            rightRotate(z);
          }
          z->parent->color=BLACK;
          z->parent->parent->color=RED;
          leftRotate(z->parent->parent);
        }
      }
    }
    root->color=BLACK;
  }

void transplant (Node* u, Node* v)
{
  if (u->parent==guard)
    root=v;
  else
  {
    if (u==u->parent->left)
      u->parent->left=v;
    else
      u->parent->right=v;
  }
  v->parent=u->parent;
}

Node* treeMinimum(Node* x)
{
  while (x->left!=guard)
    x=x->left;
  return x;
}

void deleteFixUp(Node* x)
{
  Node* w;
  while (x!=root && x->color==BLACK)
  {
    if(x==x->parent->left)
    {
      w=x->parent->right;
      if(w->color==RED)
      {
        w->color=BLACK;
        x->parent->color=RED;
        leftRotate(x->parent);
        w=x->parent->right;
      }
      if(w->left->color==BLACK && w->right->color==BLACK)
      {
        w->color=RED;
        x=x->parent;
      }
      else
      {
        if(w->right->color==BLACK)
        {
          w->left->color=BLACK;
          w->color=RED;
          rightRotate(w);
          w=x->parent->right;
        }
        w->color=x->parent->color;
        x->parent->color=BLACK;
        w->right->color=BLACK;
        leftRotate(x->parent);
        x=root;
      }
    }
    else
    {
      w=x->parent->left;
      if(w->color==RED)
      {
        w->color=BLACK;
        x->parent->color=RED;
        rightRotate(x->parent);
        w=x->parent->left;
      }
      if(w->right->color==BLACK && w->left->color==BLACK)
      {
        w->color=RED;
        x=x->parent;
      }
      else
      {
        if(w->left->color==BLACK)
        {
          w->right->color=BLACK;
          w->color=RED;
          leftRotate(w);
          w=x->parent->left;
        }
        w->color=x->parent->color;
        x->parent->color=BLACK;
        w->left->color=BLACK;
        rightRotate(x->parent);
        x=root;
      }
    }
  }
  x->color=BLACK;
}

////////////////////////////////////////////////////////////////////////////////
public:
  bool isEmpty() const
  {
    return root==guard;
  }

  mapped_type& operator[](const key_type& key)
  {
    auto it = find (key);
    if (it.current!=guard)
      return (it.current->data).second;

    Node* z = new Node;
    z->data.first=key;

    Node* y = guard;
    Node* x = root;
    while(x!=guard)
    {
      y=x;
      if (z->data.first<x->data.first)
        x=x->left;
      else
        x=x->right;
      }
    z->parent=y;
    if (y==guard)
      root=z;
    else
    {
      if (z->data.first<y->data.first)
        y->left=z;
      else
        y->right=z;
    }
    z->left=guard;
    z->right=guard;
    z->color=RED;
    insertFixUp(z);

    x=root;
    while (x->right!=guard)
      x=x->right;
    guard->parent=x;

    counter++;
    return z->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if (root==guard)
      throw std::out_of_range ("Calling valueOf() when the map is empty");
    ConstIterator it;
    it=find(key);
    if (it.current==guard)
      throw std::out_of_range ("Calling valueOf() with nonexisting key");
    return it.current->data.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    if (root==guard)
      throw std::out_of_range ("Calling valueOf() when the map is empty");
    ConstIterator it;
    it=find(key);
    if (it.current==guard)
      throw std::out_of_range ("Calling valueOf() with nonexisting key");
    return it.current->data.second;
  }

  const_iterator find(const key_type& key) const
  {
    Node* current = root;
    ConstIterator toReturn;
    while (current!=guard)
    {
      if (current->data.first==key) break;
      if (current->data.first>key) current=current->left;
      else current=current->right;
    }
    toReturn.current=current;
    return toReturn;
  }

  iterator find(const key_type& key)
  {
    Node* current = root;
    Iterator toReturn;
    while (current!=guard)
    {
      if (current->data.first==key) break;
      if (current->data.first>key) current=current->left;
      else current=current->right;
    }
    toReturn.current=current;
    return toReturn;
  }

  void remove(const key_type& key)
  {
    auto it = find (key);
    if (it.current==guard)
      throw std::out_of_range ("Removal of nonexisting node");
    if(getSize()==0)
      return;

    Node* z = it.current;
    Node* x;
    Node* y =z;
    Color yOriginalColor = y->color;

    if (z->left==guard)
    {
      x=z->right;
      transplant(z, z->right);

    }
    else
    {
      if (z->right==guard)
        {
          x=z->left;
          transplant(z, z->left);

        }
        else
        {
          y=treeMinimum(z->right);
          yOriginalColor=y->color;
          x=y->right;
          if(y->parent==z)
            x->parent=y;
          else
          {
            transplant(y,y->right);
            y->right=z->right;
            y->right->parent=y;
          }
          transplant(z,y);
          y->left=z->left;
          y->left->parent=y;
          y->color=z->color;
        }
      }
    if (yOriginalColor==BLACK)
      deleteFixUp(x);

    delete z;
    counter--;
    x=root;
    while (x->right!=guard && root != guard)
      x=x->right;
    guard->parent=x;
  }

  void remove(const const_iterator& it)
  {
    remove(it.current->data.first);
  }

  void erase()
  {
    if(getSize()==0)
      return;
    while (root!=guard)
      remove ((root->data).first);

  }

  size_type getSize() const
  {
    return counter;
  }

  bool operator==(const TreeMap& other) const
  {
    if (counter!=other.counter)
      return 0;

    auto itThis=begin();
    for (auto it=other.begin(); it!=other.end();it++)
      {
        if (it.current->data != itThis.current->data)
          return 0;
        itThis++;
      }
      return 1;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Node* current=root;
    while (current->left!=guard  && root != guard)
      current=current->left;
    Iterator it;
    it.current=current;
    return it;
  }

  iterator end()
  {
      Iterator it;
      it.current=guard;
      return it;
  }

  const_iterator cbegin() const
  {
    Node* current=root;
    while (current->left!=guard  && root != guard)
      current=current->left;
    ConstIterator it;
    it.current=current;
    return it;
  }

  const_iterator cend() const
  {
    ConstIterator it;
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

////////////////////////////////////////////////////////////////////////////////////

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;
  friend TreeMap;

private:
  Node* current;
public:

  explicit ConstIterator()
  {
        current=NULL;
  }

  ConstIterator(const ConstIterator& other)
  {
        current=other.current;
  }

  ConstIterator& operator++()
  {
    if( current->right == NULL)
      throw std::out_of_range("Wrong pointer at operator++");

    if (current->right->right!=NULL)
    {
      current=current->right;
      while (current->left->left!=NULL)
        current=current->left;
    }
    else
    {
      if (current==current->parent->left)
        {
          current=current->parent;
          return *this;
        }
        while (current==current->parent->right)
          current=current->parent;
        current=current->parent;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    return ++(*this);
  }

  ConstIterator& operator--()
  {
    if( current->right == NULL)
    {
      if (current==current->parent)
        throw std::out_of_range("Wrong pointer at operator--");
      current=current->parent;
      return *this;
    }

      if (current->left->left!=NULL)
      {
        current=current->left;
        while (current->right->right!=NULL)
          current=current->right;
      }
      else
      {
        if (current==current->parent->right)
          {
            current=current->parent;
            return *this;
          }
          while (current==current->parent->left)
            current=current->parent;
          current=current->parent;
      }
      if( current->right == NULL)
        throw std::out_of_range("Wrong pointer at operator--");
      return *this;

  }

  ConstIterator operator--(int)
  {
    return --(*this);
  }

  reference operator*() const
  {
    if( current->right == NULL)
      throw std::out_of_range("Wrong pointer at operator*");
    return current->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return current == other.current;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

///////////////////////////////////////////////////////////////////////////////////

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

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
