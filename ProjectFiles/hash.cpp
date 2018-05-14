//hash unit information in starcraft II
using namespace std;
#include "hash.h"

//this class stores the information that is stored 
//within the hash table. It forms the nodes in
//the hash table.

HashedData::HashedData(K key, D data) {
  this->key = key;
  this->data = data;
}

K HashedData::getKey() {
  return key;
}

D HashedData::getData() {
  return data;
}

HashTable() {
  //we can change the table size later
  //I'm not sure how big it should be
  //we may need to change it to be the 
  //max units possible in game
  tableSize = 150;
  currentSize = 0;
  arr = new HashedData<K, D>*[tableSize];

  for (int i = 0; i < tableSize; i++)
    arr[i] = NULL;

  dummy = new HashedData<K, D>(-1, -1);
}

int HashTable::hashCode(K key) {
  return key % tableSize;
}

void HashTable::insertNode(K key, D data) {
  HashedData<K, D> *temp = new HasedData<K, D>(key, data);
  int index = hashCode(key);

  while (arr[index] != NULL && arr[index]->key != key)
    && arr[index]->key != -1){
    index++;
    index %= tableSize;
  }

  if (arr[index] == NULL || arr[index]->key == -1)
    currentSize++

    arr[index] = temp;
}

 D HashTable::deleteNode(int key) {
   int index = hashCode(key);

   while (arr[index] != NULL)
   {
     if (arr[index]->key == key) {
       hashedData<K, D> *temp = arr[index];
       arr[index] = dummy;
       currentSize--;
       return temp->value;
     }
     index++;
     index %= tableSize;
   }
   return NULL;
 }

 //used to find data within the hash table
 D HashTable::getData(int key) {
   int index = hashCode(key);

   while (arr[index] != NULL)
   {
     if (arr[index]->key == key)
       return arr[index]->value;
     index++;
     index %= tableSize;
   }
   return NULL;
 }

 int HashTable::sizeofTable() {
   return currentSize;
 }

 bool HashTable::isEmpty() {
   if (currentSize == 0)
     return true;
 }

 //function to display stored info
 //mainly for testing
 void HashTable::display() {
   //ToDo
 }
