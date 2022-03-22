#include "Attack.h"
#include "MonoAlphabeticSubs.h"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using std::cin;
using std::cout;
using std::flush;
using std::ifstream;
using std::make_pair;
using std::pair;
using std::string;
using std::unordered_map;

enum { ALPHABETS = 26 };

// This function considers input taken through keys/substitutions.txt in the
// following format:
// A etr B etrs C fdkj D ? E * F ? G . H * I . J * ...
// Here are the meanings of these symbols:
// A etr : letter A in cipher text can only map to e, t, or r
// D ?   : letter D will be defined later
// E *   : letter E can map to any letter
// G .   : letter G can map to any letter not mentioned in all used input files
void getInputNormally(char known[ALPHABETS], string reverseKey[ALPHABETS]) {
  cout << "Note that keys in keys/substitutions.txt is used" << endl;
  ifstream file("keys/substitutions.txt");
  char cur;
  string tmp;
  while (file >> cur >> tmp) {
    if (tmp[0] == '?')
      continue;
    // The current letter should not have a previous description of its
    // substitution
    if (!known[cur - 'A']) {
      reverseKey[cur - 'A'] += tmp;
      // -1 tells other functions that this letter was defined here
      known[cur - 'A'] = -1;
    }
  }
}

// This function considers input taken through keys/freqSubs.txt in the
// following format:
// etr etrs fdkj * . * . * ...
// Here are the meanings of these symbols:
// etr  : the letter with the highest frequency in cipher text can only map to
//        e, t, or r
// etrs : the second letter with the highest frequency can only map to e, t, r,
//        or s
// *    : the corresponding letter can map to any letter
// .    : the corresponding letter can map to any letter not mentioned in all
//        used input files
void getInputUsingFrequencies(const char known[ALPHABETS],
                              string reverseKey[ALPHABETS],
                              const Attack &hack) {
  cout << "Note that keys in keys/freqSubs.txt is used" << endl;
  ifstream file("keys/freqSubs.txt");
  string sorted = hack.getCharactersSortedByFrequencies(), txt;
  int index = 0;
  while (file >> txt) {
    // The letter should not be known previously
    while (known[sorted[index] - 'A'])
      index++;
    reverseKey[sorted[index] - 'A'] = txt;
    index++;
  }
}

// This function computes the N-grams in the cipher text and prints them to a
// file
void getMostCommon(ofstream &file, const string &text, const int &count,
                   int atLeast = 1) {
  file << count << endl;
  unordered_map<string, int> freq;
  int i = count - 1;
  for (int n = text.size(); i < n; i++)
    freq[text.substr(i - count + 1, count)]++;

  int n = freq.size();
  pair<int, string> *vec = new pair<int, string>[n];
  i = 0;
  for (const auto &it : freq)
    vec[i++] = make_pair(it.second, it.first);
  sort(vec, vec + n);

  i = n - 1;
  int mx = vec[i].first;
  while (i >= 0 && (vec[i].first == mx || atLeast > 0)) {
    file << vec[i].second << ' ' << vec[i].first << endl;
    i--;
    atLeast--;
  }
  delete[] vec;
}

// This function only takes the cipher text from inputs/cipherInput.txt. You can
// also find the i-th letter in the cipher text
string getText() {
  cout << "Note that cipher text in inputs/cipherInput.txt is used" << endl;
  ifstream file("inputs/cipherInput.txt");
  string text;
  file >> text;
  cout << text << endl;
  while (true) {
    int id;
    cout << "Do you need any letter? Enter its index (1-based) or 0 if you "
         << "don't need one:" << endl;
    cin >> id;
    if (!id)
      break;
    cout << text[id - 1] << endl;
  }
  return text;
}

// This function is merely a test input for testing the Attack tool
string generateInput(bool generateKey = 0) {
  cout << "Note that plain text in inputs/input.txt is used" << endl;
  ifstream file("inputs/input.txt");
  string text, tmp;
  while (file >> tmp)
    text += tmp;
  file.close();

  MonoAlphabeticSubs tool;
  if (generateKey) {
    ofstream outKey("keys/key.txt");
    outKey << tool.getKey() << endl;
  } else {
    cout << "Note that the key in keys/key.txt is used for encryption" << endl;
    file.open("keys/key.txt");
    file >> tmp;
    bool ok = tool.setKey(tmp);
    if (!ok) {
      cout << "Key is not correct..." << endl;
      exit(-1);
    }
  }

  return tool.encrypt(text);
}

// This function accepts input in the following format:
// T t A r Z m ...
// It only considers the cipher letter and its corresponding deciphered letter
void getKnownLetters(char known[ALPHABETS], string reverseKey[ALPHABETS]) {
  cout << "Note that keys in keys/knownLetters.txt is used" << endl;
  ifstream file("keys/knownLetters.txt");
  char letter, correspond;
  while (file >> letter >> correspond) {
    known[letter - 'A'] = correspond;
    reverseKey[letter - 'A'] += correspond;
  }
}

// This function tries to understand the symbols described in different input
// files
void decodeKey(string reverseKey[ALPHABETS]) {
  bool taken[ALPHABETS] = {};
  for (int i = 0; i < ALPHABETS; i++) {
    if (reverseKey[i].empty()) {
      reverseKey[i].push_back('*');
      continue;
    }
    for (const char &c : reverseKey[i])
      if ('a' <= c && c <= 'z')
        taken[c - 'a'] = true;
  }

  for (int i = 0; i < ALPHABETS; i++) {
    if (reverseKey[i][0] == '*') {
      reverseKey[i] = string(ALPHABETS, 'a');
      for (int j = 1; j < ALPHABETS; j++)
        reverseKey[i][j] += j;
    } else if (reverseKey[i][0] == '.') {
      reverseKey[i].clear();
      for (int j = 0; j < ALPHABETS; j++)
        if (!taken[j])
          reverseKey[i] += char('a' + j);
    }
  }
}

int main() {
  Attack hack;
  hack.insertFromFileIntoDictionary("dicts/engmix_edited.txt");
  string text = getText();
  hack.setCipherText(text);

  cout << "Enter the maximum length you need for the most common substrings (0 "
       << "if not needed): " << flush;
  int maxLength;
  cin >> maxLength;
  if (maxLength) {
    cout << "Enter number of substrings for every length: " << flush;
    int num;
    cin >> num;
    ofstream file("outputs/mostCommon.txt");
    for (int i = 1; i <= maxLength; i++)
      getMostCommon(file, text, i, num);
  }

  string reverseKey[ALPHABETS];
  char known[ALPHABETS] = {};
  getKnownLetters(known, reverseKey);

  cout << "Enter type of input used (1/2/3):\n\t1- Get input normally from "
       << "keys/substitutions.txt\t\"A etr B sq C ...\"\n\t2- Get input in "
       << "descending order of frequencies from keys/freqSubs.txt\t\"etr sq "
       << "...\"\n\t3- Get input using both methods with "
       << "keys/substitutions.txt being considered first" << endl;
  int method;
  cin >> method;
  if (method == 1)
    getInputNormally(known, reverseKey);
  else if (method == 2)
    getInputUsingFrequencies(known, reverseKey, hack);
  else {
    getInputNormally(known, reverseKey);
    getInputUsingFrequencies(known, reverseKey, hack);
  }
  decodeKey(reverseKey);

  cout << "Enter type of output needed (1/2/3):\n\t1- Get every possible "
       << "output separated by words in outputs/output.txt, and sorted in "
       << "outputs/sortedOutput.txt\t\"he is clever ...\"\n\t2- Get only "
       << "complete sentences if they may be correct (not separated by words) "
       << "in outputs/outputOnce.txt\t\"heisclever...\"\n\t3- Get only "
       << "complete sentences in outputs/outputMax.txt, but some letters "
       << "may be skipped\t\"he -- clever ...\"" << endl;
  cin >> method;
  if (method == 1) {
    cout << "Set threshold value for number of matched letters: " << flush;
    int threshold;
    cin >> threshold;

    hack.setFile("outputs/output.txt");
    int numOfKeys = hack.generateKeysAndMatch(reverseKey, threshold);
    cout << numOfKeys << " keys found" << endl;

    string *vec = new string[numOfKeys];
    pair<int, int> *len = new pair<int, int>[numOfKeys];
    int tmp, i = 0;
    ifstream in("outputs/output.txt");
    while (in >> tmp) {
      len[i] = make_pair(tmp, i);
      getline(in, vec[i]);
      i++;
    }
    ofstream out("outputs/sortedOutput.txt");
    sort(len, len + numOfKeys);
    while (i--)
      out << len[i].first << vec[len[i].second] << endl;

    delete[] vec;
    delete[] len;
  } else if (method == 2) {
    hack.setFile("outputs/outputOnce.txt");
    cout << hack.generateKeysAndMatchOnce(reverseKey) << " keys found" << endl;
  } else {
    hack.setFile("outputs/outputMax.txt");

    cout << "Set threshold value for number of matched letters: " << flush;
    int threshold;
    cin >> threshold;

    char flag = 0;
    cout << "Print spaces? (Y/n): " << flush;
    char condition;
    cin >> condition;
    if (tolower(condition) != 'n')
      flag |= 1;

    cout << "Maximize threshold value? (y/N): " << flush;
    cin >> condition;
    if (tolower(condition) == 'y')
      flag |= 2;

    cout << hack.generateKeysAndMatchMax(reverseKey, threshold, flag)
         << " keys found" << endl;
  }
  return 0;
}
