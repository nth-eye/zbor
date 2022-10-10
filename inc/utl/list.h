#ifndef UTL_LIST_H
#define UTL_LIST_H

namespace utl {

// template<class T>
// struct Node {

//     Node(T val) : val{val} 
//     {

//     }

//     ~Node() 
//     { 
//         unlink(); 
//     }

//     void push_back(Node* n) 
//     {
//         n->next = this;
//         n->prev = prev;
//         prev->next = n;
//         prev = n;
//     }

//     void unlink() 
//     {
//         Node *next_tmp = next;
//         Node *prev_tmp = prev;
//         next->prev = prev_tmp;
//         prev->next = next_tmp;
//         next = this;
//         prev = this;
//     }

//     Node *next = this; 
//     Node *prev = this;
//     T val;
// };

// template<class T>
// struct NodeIter {

//     NodeIter(Node<T> *p) : p{p} {}

//     bool operator==(const NodeIter &rhs) const  { return p == rhs.p; }
//     bool operator!=(const NodeIter &rhs) const  { return p != rhs.p; }
//     NodeIter operator++(int)
//     {
//         NodeIter tmp = *this; 
//         ++(*this); 
//         return tmp;
//     }
//     NodeIter& operator++()
//     {
//         p = p->next;
//         return *this;
//     }
//     T& operator*() const    { return p->val; }
//     T* operator->() const   { return &p->val; }
// protected:
//     Node *p;
// };

// template<class T>
// struct List {

//     ~List() { clear(); }

//     NodeIter<T> begin() { return list.next; }
//     NodeIter<T> end()   { return &list; }

//     bool empty() const  { return list.next == &list; }

//     void push_back(Node<T> &val) 
//     { 
//         list.push_back(new Node<T>(val)); 
//     }

//     void erase(NodeIter<T const> i) 
//     { 
//         delete i.node(); 
//     }

//     void clear() 
//     {
//         while (!empty())
//             erase(begin());
//     }
// private:
//     Node<T> list;
// };

}

#endif