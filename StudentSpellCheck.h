#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <cctype>

const int arrCount = 27;
class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct Node {
        Node* next[arrCount];
        bool endOfWord;
        Node(bool val)
        {
            endOfWord = val;
            for (int i = 0; i < arrCount; i++)
                next[i] = nullptr;
        }
    };
    class Trie {
        public:
        Trie(){head = new Node(false);}
        ~Trie(){clearTrie(head);}
        void clearTrie(Node* ptr)
        {
            for (int i = 0; i < arrCount; i++)
                if (ptr -> next[i] != nullptr) clearTrie(ptr -> next[i]);
            delete ptr;
        }
        void insert(std::string text)
        {
            if (text.empty()) return;
            for (int i = 0; i < text.size(); i++)
                text.at(i) = tolower(text.at(i));
            Node* ptr = head;
            for (int i = 0; i < text.size(); i++)
            {
                if (!isalpha(text.at(i)) && text.at(i) != '\'') continue;
                int arr = letterNum(text.at(i));
                if (ptr -> next[arr] == nullptr)
                    ptr -> next[arr] = new Node(false);
                ptr = ptr -> next[arr];
            }
            ptr -> endOfWord = true;
        }
        bool exists(std::string checker)
        {
            Node* ptr = head;
            for (int i = 0; i < checker.size(); i++)
            {
                int arr = letterNum(checker.at(i));
                if (ptr -> next[arr] == nullptr) return false;
                ptr = ptr -> next[arr];
            }
            if (ptr -> endOfWord) return true;
            return false;
        }
        int letterNum(char ch)
        {
            switch (ch){
                case 'a':
                    return 0;
                case 'b':
                    return 1;
                case 'c':
                    return 2;
                case 'd':
                    return 3;
                case 'e':
                    return 4;
                case 'f':
                    return 5;
                case 'g':
                    return 6;
                case 'h':
                    return 7;
                case 'i':
                    return 8;
                case 'j':
                    return 9;
                case 'k':
                    return 10;
                case 'l':
                    return 11;
                case 'm':
                    return 12;
                case 'n':
                    return 13;
                case 'o':
                    return 14;
                case 'p':
                    return 15;
                case 'q':
                    return 16;
                case 'r':
                    return 17;
                case 's':
                    return 18;
                case 't':
                    return 19;
                case 'u':
                    return 20;
                case 'v':
                    return 21;
                case 'w':
                    return 22;
                case 'x':
                    return 23;
                case 'y':
                    return 24;
                case 'z':
                    return 25;
                case '\'':
                    return 26;
                default:
                    return -1;
            }
        }
        Node* head;
       
    };
    Trie m_tree;
};

#endif  // STUDENTSPELLCHECK_H_
