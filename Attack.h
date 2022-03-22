#ifndef ATTACK_HEADER
#define ATTACK_HEADER

#include "Trie.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

using std::endl;
using std::ifstream;
using std::max;
using std::ofstream;
using std::string;
using std::vector;

/**
 * This tool aims to help cryptanalyst in breaking cipher texts.
 * It searches through a given dictionary using 3 possible ways.
 *
 * The first one tries to get all possible matches within the dictionary and
 * prints to a given file all possible sentences. It is not necessary in this
 * approach that the whole sentence is matched. This can be useful in small
 * texts such as one sentence or even 5 words. A threshold value can be set
 * to print only the matches that have at least this number of letters. It is
 * implemented using a backtracking (recursive) approach.
 *
 * The second one tries to match the whole sentence, and prints to a given
 * file one of the possible sentences. This can be useful if you are checking
 * for the validity of a key to make at least one sentence. It is implemented
 * using a dynamic programming approach.
 *
 * The third and last one tries to match the whole sentence but it can skip
 * some letters from the sentence. It searches for the maximum possible letter
 * matches and prints the corresponding string with '-' replacing the skipped
 * letters. A threshold value can be set to print only the matches that have at
 * least this number of letters. A flag can be used to add some features to the
 * printed text. The first bit is responsible for printing spaces. The second
 * bit is responsibe for maximizing the number of matches while using different
 * keys. This way can be useful if you want to try only some of the given
 * letters while skipping others in order to get some useful possible words. It
 * is also implemented using a dynamic programming approach.
 **/

class Attack {
private:
  enum { ALPHABETS = 26 };
  trie *dictionary;
  string cipherText, plainText, possibleKey[ALPHABETS];
  int **visitTime, **savedAnswers, stringSize, timestamp;
  char reverseKey[ALPHABETS];
  bool taken[ALPHABETS];
  ofstream file;

  // The first way
  int generateKeysAndMatch(int index, trie *current, int spaces,
                           const int &threshold);

  // The second way
  void generateKeysAndMatchOnce(int letter);
  bool matchByDictionaryOnce(int index, trie *current);
  void buildMatchesOnce();

  // The third way
  void generateKeysAndMatchMax(int letter, int &threshold, const char &flag);
  int matchByDictionaryMax(int index, trie *current);
  void buildMatchesMax(int &threshold, const char &flag);

  int *getFrequencies() const;

public:
  Attack();
  void insertFromFileIntoDictionary(const string &fileName);

  int generateKeysAndMatch(const string possibleSubs[ALPHABETS],
                           const int &threshold);

  int generateKeysAndMatchOnce(const string possibleSubs[ALPHABETS]);

  int generateKeysAndMatchMax(const string possibleSubs[ALPHABETS],
                              int &threshold, const char &flag);

  void setCipherText(const string &cipher);
  void setFile(const string &fileName);
  string getCharactersSortedByFrequencies() const;
  ~Attack();
};
#endif
