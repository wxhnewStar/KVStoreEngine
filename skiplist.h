
//
// Created by shainvol on 2022/3/15.
//

#ifndef KVSTOREENGINE_SKIPLIST_H
#define KVSTOREENGINE_SKIPLIST_H


#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>

/* ------ Class template to implement node ------- */
/* ----------------------------------------------- */
template <typename K, typename  V>
class Node {
public:
    Node() {}

    Node (K,V,int);

    ~Node();

    K get_key() const;

    V get_value() const;

    void set_value(V);

private:
    K key;
    V value;
public:
    Node<K,V> **forward;
    int node_level;
};

template <typename K, typename  V>
Node<K,V>::Node(K k,V v,int level)
        :key(k),value(v),node_level(level) {
    // array index is range for 0 - level
    this->forward = new Node<K,V>*[level+1];

    // Fill forward array with nullptr
    memset(this->forward, 0 , sizeof(Node<K,V>*) * (level+1));
}

template<typename K, typename V>
Node<K, V>::~Node() {
    delete []forward;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return key;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return value;
}

template<typename K, typename V>
void Node<K, V>::set_value(V new_value) {
    value = new_value;
}



/* -----------design for compare key ------------- */
/* ----------------------------------------------- */
template <typename T>
bool KeyCompare(const T& key1,const T& key2) {
    return key1 == key2;
}
template<>
bool KeyCompare<double>(const double& key1,const double& key2) {
    return abs(key1 - key2) < 1e-8;
}
template<>
bool KeyCompare<float>(const float& key1,const float& key2) {
    return abs(key1 - key2) < 1e-6;
}



/* -----Class template to implement SkipList ----- */
/* ----------------------------------------------- */
template <typename K, typename  V>
class skiplist {
public:
    typedef Node<K,V> SKNode;
public:
    explicit skiplist(int);

    ~skiplist();

    int get_random_level();

    Node<K,V>* create_node(K, V,int);

    int insert_element(K,V);

    bool search_element(K);

    void delete_element(K);

    void display_list();

    int size();

private:
    // Maximum level of the skiplist
    int _max_level;

    // current level of skiplist
    int _skip_list_level;

    // pointer to header node
    SKNode *_header;

    // total number of Nodes stored in skiplist
    int _element_count;
};
/* -----------  SkipList declaration ------------*/

template<typename K, typename V>
skiplist<K, V>::skiplist(int max_level):_max_level(max_level){
    _skip_list_level = 0;
    _element_count = 0;

    // create the header node initialize key and value with nullptr
    K *pk = new K(); V *pv = new V();
    _header = new SKNode (*pk,*pv,_max_level);
}

template<typename K, typename V>
skiplist<K, V>::~skiplist() {
    delete _header;  // 删除头结点
}

template<typename K, typename V>
int skiplist<K, V>::get_random_level() {
    int  new_level = 1;
    while (rand() % 2) {
        new_level++;
    }
    return new_level > _max_level ? _max_level : new_level;
}

template<typename K, typename V>
Node<K, V> *skiplist<K, V>::create_node(K key, V value, int level) {
    SKNode *new_node = new SKNode(key,value,level);
    return new_node;
}

/* Insert given key and value
 * return 1 means element exists
 * return 0 means insert successfully
*/

template<typename K, typename V>
int skiplist<K, V>::insert_element(K key,  V value) {
    SKNode *cur = _header;

    // create and initialize the update array
    SKNode  **update = new SKNode*[_max_level + 1];
    memset(update,0,sizeof(SKNode*) * (_max_level+1));

    // find the insert index of all exist level from highest level
    for(int i = _skip_list_level; i >= 0; i--) {
        while (cur->forward[i] != nullptr && cur->forward[i]->get_key() < key) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    // reach level 0,which level stores the data
    cur = cur->forward[0];

    // judge the position situation
    // two situations:
    if (cur != nullptr && KeyCompare(cur->get_key() , key)) {  // situation 1: has exsits
        std::cout<<"key: "<< key <<" already exists"<< std::endl;
        return 1;
    }
    if (cur == nullptr || cur->get_key() != key) { // situation 2:
        int new_level = get_random_level();

        if (new_level > _skip_list_level) {
            for(int i = _skip_list_level+1 ; i <= new_level; i++) {
                update[i] = _header;
            }
            _skip_list_level = new_level;
        }

        SKNode *insert_node = create_node(key,value,new_level);

        for (int i = 0; i <= new_level; i++) {
            insert_node->forward[i] =update[i]->forward[i];
            update[i]->forward[i] = insert_node;
        }
        std::cout <<"Successfully insert Node(key: "<<key<<", value: "<<value<< ")"<<std::endl;
        ++_element_count;
    }
    delete []update;
    return 0;
}

template<typename K, typename V>
bool skiplist<K, V>::search_element(K key) {
    std::cout << "Begin searching ---------------------" << std::endl;
    SKNode *cur = _header;
    for(int i = _skip_list_level;i >= 0;i--) {
        while (cur->forward[i] != nullptr && cur->forward[i]->get_key() <key) {
            cur = cur->forward[i];
        }
    }

    // reach the position we want to search
    cur = cur->forward[0];

    // search successfully
    if (cur != nullptr && KeyCompare(cur->get_key(),key)) {
        std::cout<< "Found key: "<< key <<",match value: "<< cur->get_value() <<std::endl;
        return true;
    }

    std::cout << "Can not find this key: "<<key<< std::endl;
    return false;
}

template<typename K, typename V>
void skiplist<K, V>::delete_element(K key) {

    SKNode *cur = _header;
    SKNode **update = new SKNode*[_max_level+1];
    memset(update,0,sizeof(SKNode*)*(_max_level+1));

    // equal to insert operation,find the prev position of the node which we want to delete
    for (int i = _skip_list_level; i >=0;i--) {
        while(cur->forward[i] != nullptr && cur->forward[i]->get_key() < key) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    cur = cur->forward[0];
    if (cur != nullptr && KeyCompare(cur->get_key(),key)) {
        // has this key,then delete it from every level
        for(int i =0; i <= _skip_list_level; i++) {
            if (update[i]->forward[i] != cur)
                break;

            update[i]->forward[i] = cur->forward[i];
        }

        // renew the highest level
        while (_skip_list_level > 0 && _header->forward[_skip_list_level] == nullptr)
            --_skip_list_level;

        std::cout << "Successfully delete key: "<< key <<std::endl;
        --_element_count;
        delete cur; // wxh: previous version didn't recycle memory
    }
    delete []update;
}

template<typename K, typename V>
void skiplist<K, V>::display_list() {
    std::cout<< "\n----------- Skip List ----------"<<std::endl;
    for (int i = 0;i <= _skip_list_level; i++) {
        SKNode *node =_header->forward[i];
        std::cout << "At Level: "<< i << ": ";
        while (node != nullptr) {
            std::cout << "("<< node->get_key()<< "," << node->get_value()<<") ";
            node = node -> forward[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V>
int skiplist<K, V>::size() {
    return _element_count;
}

#endif //KVSTOREENGINE_SKIPLIST_H