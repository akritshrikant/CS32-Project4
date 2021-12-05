#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>

using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    string line = "";
    ifstream infile(dictionaryFile);
    if (!infile) return false;
    while (infile >> line)
        m_tree.insert(line);
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions)
{
    for (int i = 0; i < word.size(); i++)
        word.at(i) = tolower(word.at(i));
    string check = word;
    suggestions.clear();
    int words = 0;
    if (m_tree.exists(word)) return true;
    int sizeA = word.size();
    for (int index = 0; index < sizeA; index++)
    {
        char repl = 'a';
        check = word;
        for (int j = 0; j < arrCount; j++)
        {
            if (j == 26)
                check.at(index) = '\'';
            else check.at(index) = repl;
            if (m_tree.exists(check) && words < max_suggestions)
            {
                suggestions.push_back(check);
                words++;
            }
            if (words == max_suggestions) return false;
            if (j == 25)
                continue;
            repl++;
        }
        if (words == max_suggestions) return false;
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems)
{
    problems.clear();
    string word = "";
    int posStart = 0;
    int posEnd;
    for (int i = 0; i < line.size(); i++)
    {
        char ch = tolower(line.at(i));
        if (!isalpha(ch) && ch != '\'')
        {
            if (word.empty())
            {
                posStart = i + 1;
                continue;
            }
            else if (!m_tree.exists(word))
            {
                posEnd = i;
                Position num = {posStart, posEnd};
                problems.push_back(num);
                posStart = i + 1;
            }
            else
                posStart = i + 1;
            word = "";
        }
        else
            word += ch;
    }
    if (!word.empty())
        if (!m_tree.exists(word))
        {
            posEnd = line.size() - 1;
            Position num = {posStart, posEnd};
            problems.push_back(num);
        }
}
