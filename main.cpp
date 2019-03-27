#include <cstddef>
#include <cstdlib>
#include <string>
#include<chrono>
#include<iostream>


#include "TreeMap.h"
#include "HashMap.h"

namespace
{

template <typename K, typename V>
using TreeMap = aisdi::TreeMap<K, V>;

template <typename K, typename V>
using HashMap = aisdi::HashMap<K, V>;

void addTreeMapTest(std::size_t repeatCount)
{
  TreeMap<long int,int> collection;

  auto start = std::chrono::system_clock::now();
  for (std::size_t i = 0; i < repeatCount; ++i)
     collection[i]=i;
  auto done = std::chrono::system_clock::now();
  std::cout<<"TreeMap add time: "<<(done-start).count()<<'\n';

}

void addHashMapTest(std::size_t repeatCount)
{
  HashMap<long int,int> collection1;

  auto start = std::chrono::system_clock::now();
  for (std::size_t i = 0; i < repeatCount; ++i)
    collection1[i]=i;
  auto done = std::chrono::system_clock::now();
  std::cout<<"HashMap add time: "<<(done-start).count()<<'\n';
}


void valueOfTreeMapTest(std::size_t repeatCount)
{
    TreeMap<long int,int> collection;

    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i]=i;

    auto start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; ++i)
        collection.valueOf(i);
    auto done = std::chrono::system_clock::now();
    std::cout<<"TreeMap valueOf time: "<<(done-start).count()<<'\n';
}

void valueOfHashMapTest(std::size_t repeatCount)
{
    HashMap<long int,int> collection;

    for (std::size_t i = 0; i < repeatCount; ++i)
        collection[i]=i;

    auto start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; ++i)
        collection.valueOf(i);
    auto done = std::chrono::system_clock::now();
    std::cout<<"HashMap valueOf time: "<<(done-start).count()<<'\n';
}

} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  addTreeMapTest(repeatCount);
  addHashMapTest(repeatCount);
  valueOfTreeMapTest(repeatCount);
  valueOfHashMapTest(repeatCount);

  return 0;
}
