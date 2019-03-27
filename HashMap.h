#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

#include<vector>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
    std::vector<value_type> *mapa; //tablica wektorow
    size_t TABLE_SIZE;
    size_t Size;

size_t Hash(const key_type &key) const
    {
        std::hash <key_type> make_hash;
        return ((make_hash(key))% TABLE_SIZE);
    }

public:

  HashMap() : mapa(nullptr), TABLE_SIZE(16384), Size(0)
  {
    mapa=new std::vector<value_type>[TABLE_SIZE];
  }

  HashMap(std::initializer_list<value_type> list) : HashMap()
  {
    for(auto it=list.begin();it!=list.end();++it)
        this->operator[](it->first)=it->second;
  }

  HashMap(const HashMap& other) : HashMap()
  {
    for(auto it=other.begin();it!=other.end();++it)
        this->operator[](it->first)=it->second;
  }

  HashMap(HashMap&& other) : HashMap()
  {
    *this=std::move(other);
  }

  ~HashMap()
  {
    delete[] mapa;
    Size=0;
  }

  HashMap& operator=(const HashMap& other)
  {
    if(*this==other) return *this;
    delete[] mapa;
    Size=0;
    mapa=new std::vector<value_type>[TABLE_SIZE];
    for(auto it=other.begin();it!=other.end();++it)
        this->operator[](it->first)=it->second;
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(*this==other) return *this;
    delete[] mapa;
    mapa=other.mapa;
    Size=other.Size;
    TABLE_SIZE=other.TABLE_SIZE;

    other.mapa=nullptr;
    other.Size=0;
    other.TABLE_SIZE=0;

    return *this;
  }

  bool isEmpty() const
  {
    return !Size;
  }

  mapped_type& operator[](const key_type& key)
  {
    int h=Hash(key);
    for(auto it=mapa[h].begin();it!=mapa[h].end();++it)
    {
        if((*it).first==key) return (*it).second;
    }
    mapa[h].push_back(value_type(key,mapped_type{}));
    ++Size;
    return mapa[h].back().second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    size_t h=Hash(key);
    for(auto it=mapa[h].begin();it!=mapa[h].end();++it)
    {
        if((*it).first==key) return (*it).second;
    }
    throw std::out_of_range("valueOf");
  }

  mapped_type& valueOf(const key_type& key)
  {
    size_t h=Hash(key);
    for(auto it=mapa[h].begin();it!=mapa[h].end();++it)
    {
        if((*it).first==key) return (*it).second;
    }
    throw std::out_of_range("valueOf");
  }

  const_iterator find(const key_type& key) const
  {
    size_t h=Hash(key);
    if(Size==0 || mapa[h].size()==0) return end();
    size_t i=0;
    for(;i<mapa[h].size();++i)
        if(mapa[h][i].first==key) break;

    ConstIterator it(mapa,h,i);
    return it;
  }

  iterator find(const key_type& key)
  {
    size_t h=Hash(key);

    if(Size==0 || mapa[h].size()==0) return end();
    size_t i=0;
    for(;i<mapa[h].size();++i)
        if(mapa[h][i].first==key) break;

    Iterator it(mapa,h,i);
    return it;
  }

  void remove(const key_type& key)
  {
    if(Size==0)
        throw std::out_of_range("remove");
    size_t h=Hash(key);
    if(mapa[h].size()==0)
        throw std::out_of_range("remove");
    size_t i=mapa[h].size()-1;
    std::vector<value_type> temp;
    for(;i>=0;--i)
    {
        if(mapa[h][i].first==key)
        {
            mapa[h].pop_back();
            --Size;
            break;
        }
        value_type popped=mapa[h].back();
        temp.push_back(popped);
        mapa[h].pop_back();
    }
    while(temp.size()>0)
    {
        mapa[h].push_back(temp.back());
        temp.pop_back();
    }
  }

  void remove(const const_iterator& it)
  {
    if(it==end())
        throw std::out_of_range("remove");
    size_t h=it.hash_index;
    const key_type &key=mapa[h][it.vec_index].first;
    remove(key);
  }

  size_type getSize() const
  {
    return Size;
  }

  bool operator==(const HashMap& other) const
  {
    if(Size!=other.Size)
        return false;
    auto it=begin();
    for(auto ito=other.begin();ito!=other.end();++ito)
    {
        if(*it!=*ito) return false;
        ++it;
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  size_t first_index() const
  {
    if(Size==0) return 0;
    size_t h=0;

    while(h<TABLE_SIZE && mapa[h].size()==0)
        h++;

    return h;
  }

  size_t last_index() const
  {
    if(Size==0) return 0;
    size_t h=TABLE_SIZE-1;

    while(h>=0 && mapa[h].size()==0)
        --h;

    return ++h;
  }

  iterator begin()
  {
    Iterator it(mapa,first_index(),0);
    return it;
  }

  iterator end()
  {
    Iterator it(mapa,last_index(),0);
    return it;
  }

  const_iterator cbegin() const
  {
    ConstIterator it(mapa,first_index());
    return it;
  }

  const_iterator cend() const
  {
    ConstIterator it(mapa,last_index());
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

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

private:
  const std::vector<value_type> *mapa;
  size_t hash_index;
  size_t vec_index;

  size_t TABLE_SIZE=16384;

  friend void HashMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const std::vector<value_type> *m=nullptr, size_t h=0, size_t v=0, size_t t=16384) : mapa(m), hash_index(h), vec_index(v), TABLE_SIZE(t) {}

  ConstIterator(const ConstIterator& other) : ConstIterator(other.mapa,other.hash_index,other.vec_index) {}

  ConstIterator& operator++()
  {
    if(mapa[hash_index].size()==0)
        throw std::out_of_range("++");
    if(mapa[hash_index].size()-1>vec_index)
    {
        vec_index++;
        return *this;
    }
    size_t current=hash_index;
    do
    {
        ++current;
    }
    while(current<TABLE_SIZE-1 && mapa[current].size()==0);

    if(current==TABLE_SIZE-1)
    {
        ++hash_index;
        vec_index=0;
    }
    else
    {
        hash_index=current;
        vec_index=0;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator temp=*this;
    operator++();
    return temp;
  }

  ConstIterator& operator--()
  {
    if(hash_index==0 && mapa[hash_index].size()==0)
        throw std::out_of_range("--");

    if(vec_index>0)
    {
        --vec_index;
        return *this;
    }
    size_t current=hash_index;
    do
    {
        --current;
    }
    while(current>=0 && mapa[current].size()==0);

    if(current<0)
        throw std::out_of_range("--");
    hash_index=current;
    vec_index=mapa[hash_index].size()-1;

    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator temp=*this;
    operator--();
    return temp;
  }

  reference operator*() const
  {
    if(vec_index>=mapa[hash_index].size())
        throw std::out_of_range("*");
    return mapa[hash_index][vec_index];
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (mapa==other.mapa && hash_index==other.hash_index && vec_index==other.vec_index);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(const std::vector<value_type> *m=nullptr, size_t h=0, size_t v=0, size_t t=16384) : ConstIterator(m,h,v,t) {}

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

#endif /* AISDI_MAPS_HASHMAP_H */
