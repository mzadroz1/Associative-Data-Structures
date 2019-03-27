#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
    struct Node
    {
        value_type *data;
        Node *parent;
        Node *left;
        Node *right;
        int balance; //wpsolczynnik rownowagi

        Node() : data(nullptr), parent(nullptr), left(nullptr), right(nullptr), balance(0) {}

    };

  Node *root;
  size_type Size;

public:
  TreeMap() : root(nullptr), Size(0) {}

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for(auto it=list.begin();it!=list.end();++it)
        this->operator[](it->first)=it->second;
  }

  TreeMap(const TreeMap& other) : TreeMap()
  {
    for(auto it=other.begin();it!=other.end();++it)
        this->operator[](it->first)=it->second;
  }

  TreeMap(TreeMap&& other) : TreeMap()
  {
    *this=std::move(other);
  }

  ~TreeMap()
  {
    remove_all(root);
    Size=0;
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this==&other) return *this;

    remove_all(root);
    Size=0;
    root=nullptr;
    for(auto it=other.begin();it!=other.end();++it)
        this->operator[](it->first)=it->second;
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(this==&other) return *this;

    remove_all(root);

    root=other.root;
    Size=other.Size;

    other.root=nullptr;
    other.Size=0;

    return *this;
  }

  bool isEmpty() const
  {
    return !Size;
  }

  mapped_type& operator[](const key_type& key)
  {
        Node *temp=new Node;
        temp->data=new value_type(key, mapped_type{});

        Node *p=root;

        if(p==nullptr)
        {
            root = temp;
            Size++;
            return temp->data->second;
        }

        while(true)
        {
            if(key==p->data->first)
            {

                    delete temp->data;
                    delete temp;
                    return p->data->second;
            }

            if(key < p->data->first)
            {
                if(p->left==nullptr)
                {
                    p->left=temp;
                    break;
                }
                p=p->left;
            }
            else
            {
                if(p->right==nullptr)
                {
                    p->right=temp;
                    break;
                }
                p=p->right;
            }
        }

        temp->parent=p;

        if(p->balance)
        {
            p->balance=0;
            Size++;
            return temp->data->second;
        }

        if(p->left == temp) p->balance=1;
        else  p->balance=-1;

        Node *p_parent=p->parent;
        bool unbalanced=false;

        while(p_parent)
        {
            if(p_parent->balance)
            {
                unbalanced = true;
                break;
            }

            if(p_parent->left==p)  p_parent->balance=1;
            else p_parent->balance=-1;

            p=p_parent;
            p_parent=p_parent->parent;
        }

        if(unbalanced)
        {
            if(p_parent->balance==1)
            {
                if(p_parent->right==p) p_parent->balance=0;

                else if(p->balance == -1)
                    LR(p_parent);
                else
                    LL(p_parent);
            }
            else
            {
                if(p_parent->left==p) p_parent->balance=0;

                else if(p->balance==1)
                    RL(p_parent);
                else
                    RR(p_parent);
            }
        }
        Size++;
        return temp->data->second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const Node *current=find_node(key);
    if(current==nullptr)
        throw std::out_of_range("const_valueOf");
    return current->data->second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node *current=find_node(key);
    if(current==nullptr)
        throw std::out_of_range("valueOf");
    return current->data->second;
  }

  const_iterator find(const key_type& key) const
  {
    ConstIterator it(find_node(key),this);
    return it;
  }

  iterator find(const key_type& key)
  {
    Iterator it(find_node(key),this);
    return it;
  }

  void remove(const key_type& key)
  {
    Node *tmp=find_node(key);
    if(tmp==nullptr)
        throw std::out_of_range("remove");
    remove_node(tmp);
    delete tmp->data;
    delete tmp;
  }

  void remove(const const_iterator& it)
  {
    Node *tmp=it.node;
    if(tmp==nullptr)
        throw std::out_of_range("remove");
    remove_node(tmp);
    delete tmp->data;
    delete tmp;
  }

  size_type getSize() const
  {
    return Size;
  }

  bool operator==(const TreeMap& other) const
  {
    if(Size!=other.Size) return false;
    for(auto it=begin(),ito=other.begin();it!=end(),ito!=end();++it,++ito)
    {
        if(*it!=*ito) return false;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    Iterator it(find_minimum(root),this);
    return it;
  }

  iterator end()
  {
    Iterator it(nullptr,this);
    return it;
  }

  const_iterator cbegin() const
  {
    ConstIterator it(find_minimum(root),this);
    return it;
  }

  const_iterator cend() const
  {
    ConstIterator it(nullptr,this);
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

private:

  void remove_all(Node *A)
    {
        if(A==nullptr) return;
        else
        {
            remove_all(A->left);
            remove_all(A->right);
            delete A->data;
            delete A;
        }
    }

void RR(Node *A)
{
    Node *B=A->right;
    Node *p=A->parent;

    A->right=B->left;

    if(A->right)
        A->right->parent=A;

    B->left=A;
    B->parent=p;
    A->parent=B;

    if(p)
    {
        if(p->left==A) p->left=B;
        else p->right=B;
    }
    else root=B;

    if(B->balance==-1) A->balance=B->balance=0;
    else {A->balance=-1; B->balance=1;}
}

void LL(Node *A)
{
    Node *B=A->left;
    Node *p=A->parent;

    A->left=B->right;

    if(A->left)
        A->left->parent=A;

    B->right=A;
    B->parent=p;
    A->parent=B;

    if(p)
    {
        if(p->left==A) p->left=B;
        else p->right=B;
    }
    else root=B;

    if(B->balance==1) A->balance=B->balance=0;
    else {A->balance=1; B->balance=-1;}
}

void RL(Node *A)
{
    Node *B=A->right;
    Node *C=B->left;
    Node *p=A->parent;

    B->left=C->right;

    if(B->left) B->left->parent=B;

    A->right=C->left;
    if(A->right) A->right->parent=A;

    C->left=A;
    C->right=B;
    A->parent=B->parent=C;
    C->parent=p;

    if(p)
    {
        if(p->left==A) p->left=C;
        else p->right=C;
    }
    else root=C;

    if(C->balance==-1) A->balance=1;
    else A->balance=0;

    if(C->balance==1) B->balance=-1;
    else B->balance=0;

    C->balance=0;
}

void LR(Node *A)
{
    Node *B=A->left;
    Node *C=B->right;
    Node *p=A->parent;

    B->right=C->left;

    if(B->right) B->right->parent=B;

    A->left=C->right;
    if(A->left) A->left->parent=A;

    C->left=B;
    C->right=A;
    A->parent=B->parent=C;
    C->parent=p;

    if(p)
    {
        if(p->left==A) p->left=C;
        else p->right=C;
    }
    else root=C;

    if(C->balance==-1) B->balance=1;
    else B->balance=0;

    if(C->balance==1) A->balance=-1;
    else A->balance=0;

    C->balance=0;
}


Node* find_node(const key_type& key) const
{
    Node* node=root;
    while(node!=nullptr)
    {
        if(key < node->data->first) node=node->left;
        else if(key > node->data->first) node=node->right;
        else break;
    }
    return node;
}

Node* find_minimum(Node *node) const
{
    if(node!=nullptr)
        while(node->left!=nullptr)
            node=node->left;
    return node;
}

Node* find_maximum(Node *node) const
{
    if(node!=nullptr)
    {
            while(node->right!=nullptr)
            node=node->right;
    }
        return node;
}

Node* prev_node(Node *A) const
{
    Node *B;
    if(A==nullptr) return A;
    if(A->left!=nullptr)
    {
        A=A->left;
        while(A->right!=nullptr) A=A->right;
    }
    else
        do
        {
            B=A;
            A=A->parent;
        } while(A && A->right!=B);

    return A;
}

Node* next_node(Node *A) const
{
    Node *B;
    if(A==nullptr) return A;
    if(A->right!=nullptr) return find_minimum(A->right);
    B=A->parent;
    while(B!=nullptr && A==B->right)
    {
        A=B;
        B=B->parent;
    }
    return B;
}

Node* remove_node(Node *A)
{
    Size--;
    Node *tmp;
    Node *B;
    Node *C;

    bool x; //false: node ma obu synow

    if(A->left && A->right)
    {
        B=remove_node(prev_node(A));
        x=false;
    }
    else
    {
        if(A->left)
        {
        B=A->left;
        A->left=nullptr;
        }
        else
        {
        B=A->left;
        A->left=nullptr;
        }
        A->balance=0;
        x=true;
    }

    if(B)
    {
        B->parent=A->parent;
        B->left=A->left;
        if(B->left) B->left->parent=B;
        B->right=A->right;
        if(B->right) B->right->parent=B;
        B->balance=A->balance;
    }
    if(A->parent)
    {
        if(A->parent->left==A) A->parent->left=B;
        else A->parent->right=B;
    }
    else root=B;

    if(x)
    {
        C=B;
        C=A->parent;
        while(B)
        {
            if(!B->balance)
            {
                if(B->left==C) B->balance=-1;
                else B->balance=1;
                break;
            }
            else
            {
                if(((B->balance==1) && (B->left==C)) || ((B->balance==-1) && (B->right==C)))
                {
                    B->balance=0;
                    C=B;
                    B=B->parent;
                }
                else
                {
                    if(B->left==C) tmp=B->right;
                    else tmp=B->left;
                    if(!tmp->balance)
                    {
                        if(B->balance==1) LL(B);
                        else RR(B);
                        break;
                    }
                    else if(B->balance==tmp->balance)
                    {
                        if(B->balance==1) LL(B);
                        else RR(B);
                        C=tmp;
                        B=tmp->parent;
                    }
                    else
                    {
                        if(B->balance==1) LR(B);
                        else RL(B);
                        C=B->parent;
                        B=C->parent;
                    }
                }
            }
        }
    }
    return A;
}




};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;
private:

  Node *node;
  const TreeMap *tree;

  friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(Node* n=nullptr, const TreeMap *t=nullptr) : node(n), tree(t)
  {}

  ConstIterator(const ConstIterator& other) : ConstIterator(other.node,other.tree) {}

  ConstIterator& operator++()
  {
    if(node==nullptr)
        throw std::out_of_range("++");

    node=tree->next_node(node);
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator tmp=*this;
    operator++();
    return tmp;
  }

  ConstIterator& operator--()
  {
    if(*this==tree->begin())
        throw std::out_of_range("--");

    if(node==nullptr)
    {
        node=tree->find_maximum(tree->root);
        return *this;
    }

    node=tree->prev_node(node);
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator tmp=*this;
    operator--();
    return tmp;
  }

  value_type& getValueType() const
  {
    value_type *value;
    value = node->data;
    return *value;
  }

  reference operator*() const
  {
    if(node==nullptr)
        throw std::out_of_range("");

    return getValueType();
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    if(node==nullptr && other.node==nullptr) return true;

    else if((node==nullptr && other.node!=nullptr) || (node!=nullptr && other.node==nullptr))
        return false;

    if(node->data==other.node->data && tree==other.tree)
        return true;
    else
        return false;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};


template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(Node* n=nullptr, const TreeMap* t=nullptr) : ConstIterator(n,t)
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

#endif /* AISDI_MAPS_MAP_H */
