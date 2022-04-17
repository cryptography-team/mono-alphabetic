#include "Trie.h"

int trie::numOfNodes = 0;

trie::trie(int dep) : valid(false), depth(dep) {
  index = trie::numOfNodes;
  trie::numOfNodes++;
  memset(child, 0, ALPHABETS * sizeof(trie *));
}

trie::trie() : trie(0) {}

void trie::addWord(const string &word) {
  trie *current = this;
  int len = word.length();
  for (int i = 0; i < len; i++) {
    // Ignore non-alphabetic characters like apostrophe
    if (word[i] < 'a' || word[i] > 'z')
      continue;
    if (current->child[word[i] - 'a'] == nullptr)
      current->child[word[i] - 'a'] = new trie(depth + i + 1);
    current = current->child[word[i] - 'a'];
  }
  current->valid = true;
}

trie *trie::getNext(char c) const { return child[c - 'a']; }

const bool &trie::isWord() const { return valid; }

const int &trie::getIndex() const { return index; }

const int &trie::getNumOfNodes() { return trie::numOfNodes; }

const int &trie::getDepth() const { return depth; }

trie::~trie() {
  for (int i = 0; i < ALPHABETS; i++) {
    if (child[i] != NULL)
      delete child[i];
  }
  trie::numOfNodes--;
}
