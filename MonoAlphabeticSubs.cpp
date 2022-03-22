#include "MonoAlphabeticSubs.h"

void MonoAlphabeticSubs::updateReverseKey() {
  for (int i = 0; i < ALPHABETS; i++)
    reverseKey[key[i] - 'A'] = i + 'a';
}

void MonoAlphabeticSubs::substituteByKey(string &text,
                                         const string &key) const {
  if (text.empty())
    return;
  char first;
  if ('A' <= text[0] && text[0] <= 'Z')
    first = 'A';
  else
    first = 'a';
  int n = text.size();
  for (int i = 0; i < n; i++)
    text[i] = key[text[i] - first];
}

inline void MonoAlphabeticSubs::damageReverseKey() { reverseKey[0] = ' '; }

inline bool MonoAlphabeticSubs::isDamaged() { return reverseKey[0] == ' '; }

MonoAlphabeticSubs::MonoAlphabeticSubs()
    : key(ALPHABETS, 'A'), reverseKey(ALPHABETS, ' ') {
  for (int i = 1; i < ALPHABETS; i++)
    key[i] += i;
  generateRandomKey();
}

string MonoAlphabeticSubs::reformat(const string &plainText) {
  string reformattedPlainText;
  int n = plainText.size();
  for (int i = 0; i < n; i++) {
    if ('a' <= plainText[i] && plainText[i] <= 'z')
      reformattedPlainText += plainText[i];
    else if ('A' <= plainText[i] && plainText[i] <= 'Z')
      reformattedPlainText += plainText[i] - 'A' + 'a';
  }
  return reformattedPlainText;
}

string MonoAlphabeticSubs::encrypt(const string &plainText) const {
  string cipherText = reformat(plainText);
  substituteByKey(cipherText, key);
  return cipherText;
}

string MonoAlphabeticSubs::decrypt(const string &cipherText) {
  if (isDamaged())
    updateReverseKey();
  string plainText = cipherText;
  substituteByKey(plainText, reverseKey);
  return plainText;
}

void MonoAlphabeticSubs::generateRandomKey() {
  std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
  shuffle(key.begin(), key.end(), rng);
  damageReverseKey();
}

string MonoAlphabeticSubs::getKey() const { return key; }

string MonoAlphabeticSubs::getReverseKey() {
  if (isDamaged())
    updateReverseKey();
  return reverseKey;
}

bool MonoAlphabeticSubs::isValidKey(const string &key) {
  if (key.size() != ALPHABETS)
    return false;
  bool exist[ALPHABETS] = {};
  for (int i = 0; i < ALPHABETS; i++) {
    if (key[i] < 'A' || key[i] > 'Z' || exist[key[i] - 'A'])
      return false;
    exist[key[i] - 'A'] = true;
  }
  return true;
}

bool MonoAlphabeticSubs::setKey(const string &key) {
  if (!isValidKey(key))
    return false;
  this->key = key;
  damageReverseKey();
  return true;
}
