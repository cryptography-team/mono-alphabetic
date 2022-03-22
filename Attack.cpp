#include "Attack.h"

int Attack::generateKeysAndMatch(int index, trie *current, int spaces,
                                 const int &threshold) {
  if (!current || index == stringSize) {
    int i = plainText.size() - 1;
    while (i >= 0 && plainText[i] != ' ')
      i--;
    if (i - spaces + 1 < threshold)
      return 0;
    file << i - spaces + 1 << ' ';
    for (int j = 0; j < i; j++)
      file << plainText[j];
    file << endl;
    return 1;
  }

  int letter = cipherText[index] - 'A', cnt = 0;

  // If the letter is not yet mapped, map it and try again
  if (reverseKey[letter] == -1) {
    for (const auto &c : possibleKey[letter]) {
      if (!taken[c - 'a']) {
        reverseKey[letter] = c;
        taken[c - 'a'] = true;
        cnt += generateKeysAndMatch(index, current, spaces, threshold);
        // remove the mapping for future use in other letters
        taken[c - 'a'] = false;
      }
    }
    reverseKey[letter] = -1;
    return cnt;
  }

  // If this can be a word, end it and try again
  if (current->isWord()) {
    plainText.push_back(' ');
    cnt += generateKeysAndMatch(index, dictionary, spaces + 1, threshold);
    plainText.pop_back();
  }

  // As long as you can use the mapping, do it and try again
  plainText.push_back(reverseKey[letter]);
  cnt += generateKeysAndMatch(index + 1, current->getNext(reverseKey[letter]),
                              spaces, threshold);
  plainText.pop_back();

  return cnt;
}

void Attack::generateKeysAndMatchOnce(int letter) {
  if (letter == ALPHABETS) {
    // Having already generated a valid key, try building a sentence
    timestamp++;
    buildMatchesOnce();
    return;
  }

  // Assign a mapping for the current letter and move forward
  for (const char &sub : possibleKey[letter]) {
    if (!taken[sub - 'a']) {
      reverseKey[letter] = sub;
      taken[sub - 'a'] = true;
      generateKeysAndMatchOnce(letter + 1);
      // remove the mapping for future use in other letters
      taken[sub - 'a'] = false;
      reverseKey[letter] = -1;
    }
  }
}

bool Attack::matchByDictionaryOnce(int index, trie *current) {
  // We reached a null state.. We can no longer continue using
  // the dictionary, so return
  if (!current)
    return false;
  // We finally reached the end of the text, so try matching
  // the last word
  if (index == stringSize)
    return current->isWord();

  const int &trieIndex = current->getIndex();
  int &ans = savedAnswers[index][trieIndex];

  // If we got here before, just return the previous answer
  if (visitTime[index][trieIndex] == timestamp)
    return ans;
  visitTime[index][trieIndex] = timestamp;

  // Assume that there is no match
  ans = false;

  // If we can end a word, try ending it
  if (current->isWord())
    ans = matchByDictionaryOnce(index, dictionary);

  // Continue by using the key
  ans = ans ||
        matchByDictionaryOnce(
            index + 1, current->getNext(reverseKey[cipherText[index] - 'A']));

  return ans;
}

void Attack::buildMatchesOnce() {
  // If there is no answer, it is not necessary to try building one
  if (!matchByDictionaryOnce(0, dictionary))
    return;

  int index = 0;
  trie *current = dictionary;
  while (index < stringSize) {
    // If we can move on by ending the word, do it
    if (current->isWord() && matchByDictionaryOnce(index, dictionary)) {
      file << ' ';
      current = dictionary;
      continue;
    }
    // Otherwise use the mapping and move to the next character in the text
    char c = reverseKey[cipherText[index] - 'A'];
    index++;
    current = current->getNext(c);
    file << c;
  }
  file << endl;
}

void Attack::generateKeysAndMatchMax(int letter, int &threshold,
                                     const char &flag) {
  if (letter == ALPHABETS) {
    // Having already generated a valid key, try building a sentence
    timestamp++;
    buildMatchesMax(threshold, flag);
    return;
  }

  // Assign a mapping for the current letter and move forward, or ignore
  // this letter if it shouldn't have a mapping
  for (const char &sub : possibleKey[letter]) {
    if (sub == '-') {
      // It shouldn't have a mapping
      reverseKey[letter] = sub;
      generateKeysAndMatchMax(letter + 1, threshold, flag);
      // remove the mapping for future use in other letters
      reverseKey[letter] = -1;
    } else if (!taken[sub - 'a']) {
      reverseKey[letter] = sub;
      taken[sub - 'a'] = true;
      generateKeysAndMatchMax(letter + 1, threshold, flag);
      // remove the mapping for future use in other letters
      taken[sub - 'a'] = false;
      reverseKey[letter] = -1;
    }
  }
}

int Attack::matchByDictionaryMax(int index, trie *current) {
  // We finally reached the end of the text, so try matching
  // the last word
  if (index == stringSize)
    return current->isWord() ? current->getDepth() : 0;

  const int &trieIndex = current->getIndex();
  int &ans = savedAnswers[index][trieIndex];

  // If we got here before, just return the previous answer
  if (visitTime[index][trieIndex] == timestamp)
    return ans;
  visitTime[index][trieIndex] = timestamp;

  // Assume that there is no other matches
  ans = 0;

  // Try skipping this letter if we are not in the middle of matching a word
  if (current == dictionary)
    ans = matchByDictionaryMax(index + 1, current);

  // If we can end a word, try ending it
  if (current->isWord())
    ans =
        max(ans, matchByDictionaryMax(index, dictionary) + current->getDepth());

  // Continue by using the key if the character can be mapped
  const char &c = reverseKey[cipherText[index] - 'A'];
  if (c != '-') {
    current = current->getNext(c);
    if (current)
      ans = max(ans, matchByDictionaryMax(index + 1, current));
  }

  return ans;
}

void Attack::buildMatchesMax(int &threshold, const char &flag) {
  int index = 0;
  trie *current = dictionary;
  int ans = matchByDictionaryMax(index, current);

  // Do not build the string if it is already below the threshold value
  if (ans < threshold)
    return;
  // Maximize the threshold if needed
  if (flag & 2)
    threshold = ans;

  file << ans << ' ';
  // The following variable is used to detect the end of a word before
  // starting to skip some letters so that a space can be inserted first
  bool last = false;

  while (index < stringSize) {
    ans = matchByDictionaryMax(index, current);

    // If we should skip this letter, do it
    if (current == dictionary &&
        ans == matchByDictionaryMax(index + 1, current)) {
      if (last) {
        last = false;
        if (flag & 1)
          file << ' ';
      }
      file << '-';
      index++;
      continue;
    }

    // If we can move on by ending the word, do it
    if (current->isWord() &&
        ans == matchByDictionaryMax(index, dictionary) + current->getDepth()) {
      last = true;
      current = dictionary;
      continue;
    }

    // Otherwise use the mapping and move to the next character in the text
    if (current == dictionary && index && (flag & 1))
      file << ' ';
    const char &c = reverseKey[cipherText[index] - 'A'];
    index++;
    current = current->getNext(c);
    file << c;
  }
  file << endl;
}

Attack::Attack() : stringSize(0), timestamp(0) {
  dictionary = new trie;
  memset(taken, 0, ALPHABETS * sizeof(bool));
  memset(reverseKey, -1, ALPHABETS * sizeof(char));
}

void Attack::insertFromFileIntoDictionary(const string &fileName) {
  ifstream file(fileName.c_str());
  string cur;
  while (file >> cur) {
    dictionary->addWord(cur);
  }
}

int Attack::generateKeysAndMatch(const string possibleSubs[ALPHABETS],
                                 const int &threshold) {
  for (int i = 0; i < ALPHABETS; i++)
    possibleKey[i] = possibleSubs[i];

  return generateKeysAndMatch(0, dictionary, 0, threshold);
}

int Attack::generateKeysAndMatchOnce(const string possibleSubs[ALPHABETS]) {
  for (int i = 0; i < ALPHABETS; i++)
    possibleKey[i] = possibleSubs[i];

  int trieSize = trie::getNumOfNodes(), numOfKeys = timestamp;

  visitTime = new int *[stringSize];
  savedAnswers = new int *[stringSize];
  for (int i = 0; i < stringSize; i++) {
    visitTime[i] = new int[trieSize];
    savedAnswers[i] = new int[trieSize];
  }

  generateKeysAndMatchOnce(0);

  numOfKeys = timestamp - numOfKeys;

  for (int i = 0; i < stringSize; i++) {
    delete[] visitTime[i];
    delete[] savedAnswers[i];
  }
  delete[] visitTime;
  delete[] savedAnswers;

  return numOfKeys;
}

int Attack::generateKeysAndMatchMax(const string possibleSubs[ALPHABETS],
                                    int &threshold, const char &flag) {
  for (int i = 0; i < ALPHABETS; i++)
    possibleKey[i] = possibleSubs[i];

  int trieSize = trie::getNumOfNodes(), numOfKeys = timestamp;

  visitTime = new int *[stringSize];
  savedAnswers = new int *[stringSize];
  for (int i = 0; i < stringSize; i++) {
    visitTime[i] = new int[trieSize];
    savedAnswers[i] = new int[trieSize];
  }

  generateKeysAndMatchMax(0, threshold, flag);

  numOfKeys = timestamp - numOfKeys;

  for (int i = 0; i < stringSize; i++) {
    delete[] visitTime[i];
    delete[] savedAnswers[i];
  }
  delete[] visitTime;
  delete[] savedAnswers;

  return numOfKeys;
}

void Attack::setCipherText(const string &cipher) {
  cipherText = cipher;
  stringSize = cipher.size();
}

void Attack::setFile(const string &fileName) {
  if (file.is_open())
    file.close();
  file.open(fileName.c_str());
}

string Attack::getCharactersSortedByFrequencies() const {
  string characters(ALPHABETS, 'A');
  for (int i = 1; i < ALPHABETS; i++)
    characters[i] += i;
  int *freq = getFrequencies();

  // Using a lambda function inside the sorting function to override the
  // comparing function
  sort(characters.begin(), characters.end(), [&](const char &a, const char &b) {
    return freq[a - 'A'] > freq[b - 'A'];
  });
  delete[] freq;

  return characters;
}

int *Attack::getFrequencies() const {
  int *freq = new int[ALPHABETS];
  for (const char &c : cipherText)
    freq[c - 'A']++;
  return freq;
}

Attack::~Attack() { delete dictionary; }
