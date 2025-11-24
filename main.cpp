#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <queue>
using namespace std;



class Trie {

    public: 
        unordered_map<string, Trie*> child;
        bool isLeaf;

        Trie() {
            isLeaf = false;
        }

        void insert(Trie* root, string key);
};



void insert(Trie* root, const string& key) {
  
    Trie* curr = root;
    stringstream ss; 
    string word="";
    while(ss>>word) {
        if (curr->child[word] == nullptr) {
          
            Trie* newNode = new Trie();
          
            curr->child[word] = newNode;
        }
      
        curr = curr->child[word];
    }

    curr->isLeaf = true;
}