#ifndef MonoAlphabeticSubs_Header
#define MonoAlphabeticSubs_Header
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
using std::string;
class MonoAlphabeticSubs {
private:
  enum { ALPHABETS = 26 };
  string key, reverseKey;
  void updateReverseKey();
  void substituteByKey(string &text, const string &key) const;
  inline void damageReverseKey();
  inline bool isDamaged();

public:
  MonoAlphabeticSubs();
  static string reformat(const string &plainText);
  string encrypt(const string &plainText) const;
  string decrypt(const string &cipherText);
  void generateRandomKey();
  string getKey() const;
  string getReverseKey();
  static bool isValidKey(const string &key);
  bool setKey(const string &key);
};
#endif
