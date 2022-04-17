#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

#include <cstring>
#include <string>

using std::string;

/**
 * This data structure is used to do incremental search on words.
 * First, store these words using addWord function. After that,
 * whenever you search for this word, you can use getNext function
 * until you reach the end of the word and check if it is a valid word
 * using isWord function.
 **/

class trie {
private:
  enum { ALPHABETS = 26 };
  trie *child[ALPHABETS];
  bool valid;
  int index, depth;
  static int numOfNodes;
  // Users cannot define the depth of nodes. This constructor is for use only
  // inside the class
  trie(int dep);

public:
  trie();
  void addWord(const string &word);
  // Move within the data structure by going to children of the caller's node
  trie *getNext(char c) const;
  // If this node completes a word, it returns true
  const bool &isWord() const;
  const int &getIndex() const;
  static const int &getNumOfNodes();
  const int &getDepth() const;
  ~trie();
};
#endif
