#pragma once


//store whatever you want
template<typename K, typename D>

//this class stores the information that is stored 
//within the hash table. It forms the nodes in
//the hash table.
class HashedData {

private:
  K key;  //where to find the data
  D data; //actual data

public:
  HashedData(K key, D data);
  K getKey();
  D getData();
};

//hash structure
public class HashTable {

};

//actual hash structure
public class HashTable {

private:
  HashedData<K, D> **arr;
  int tableSize;
  int currentSize;
  HashedData<K, D> *dummy;

public: 
  HashTable();
  int hashCode(K key);
  void insertNode(K key, D data);
  D deleteNode(int key);
  D getData(int key);
  int sizeofTable();
  bool isEmpty();
  void display();
};