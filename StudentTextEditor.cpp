#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo)
{
    curRow = 0;
    letterIndex = 0;
    maxRow = 0;
    docU.push_back("");
    it = docU.begin();
    undoFunc = false;
}

StudentTextEditor::~StudentTextEditor()
{
    reset();
}

bool StudentTextEditor::load(std::string file)
{
    ifstream infile(file);
    if (!infile) return false;
    reset();
    string liner;
    while (getline(infile, liner))
    {
        for (int i = 0; i < liner.size(); i++)
        {
            char ins = liner.at(i);
            if (ins == '\r')
                continue;
            change(true);
            insert(ins);
        }
        enter();
    }
    curRow = 0;
    letterIndex = 0;
    it = docU.begin();
    return true;
}

bool StudentTextEditor::save(std::string file) {
    ofstream outfile(file);
    if (!outfile) return false;
    for (auto iter = docU.begin(); iter != docU.end(); iter++)
        outfile << (*iter) << endl;
    return true;
}

void StudentTextEditor::reset() {
    docU.clear();
    curRow = 0;
    maxRow = 0;
    letterIndex = 0;
    docU.push_back("");
    it = docU.begin();
    TextEditor::getUndo() -> clear();
}

void StudentTextEditor::move(Dir dir) {
    list<string>::iterator next = it;
    next++;
    list<string>::iterator prev = it;
    prev--;
    switch (dir){
        case (Dir::UP):
            if (curRow == 0) break;
            if (letterIndex > (*prev).size())
            {
                letterIndex = (*prev).size();
                it--;
                curRow--;
            }
            else
            {
                it--;
                curRow--;
            }
            break;
        case (Dir::DOWN):
            if (curRow == maxRow) break;
            if (letterIndex > (*next).size())
            {
                letterIndex = (*next).size();
                it++;
                curRow++;
            }
            else
            {
                it++;
                curRow++;
            }
            break;
        case (Dir::LEFT):
            if (letterIndex == 0)
            {
                if (curRow == 0) break;
                it--;
                letterIndex = (*it).size();
                curRow--;
                break;
            }
            letterIndex--;
            break;
        case (Dir::RIGHT):
            if (letterIndex >= (*it).size())
            {
                if (curRow == maxRow) break;
                it++;
                letterIndex = 0;
                curRow++;
                break;
            }
            letterIndex++;
            break;
        case (Dir::HOME):
            letterIndex = 0;
            break;
        case (Dir::END):
            letterIndex = (*it).size();
            break;
        default:
            break;
    }
}

void StudentTextEditor::del() {
    if (letterIndex == (*it).size())
    {
        if (curRow == maxRow) return;
        list<string>::iterator next = it; next++;
        if (!isUndoFunc())
            TextEditor::getUndo() -> submit(Undo::Action::JOIN, curRow, letterIndex);
        (*it) += (*next);
        docU.erase(next);
        maxRow--;
        change(false);
        return;
    }
    char cha = (*it).at(letterIndex);
    if (!isUndoFunc())
        TextEditor::getUndo() -> submit(Undo::Action::DELETE, curRow, letterIndex, cha);
    change(false);
    (*it).erase(letterIndex, 1);
    change(false);
}

void StudentTextEditor::backspace() {
    if (letterIndex > 0)
    {
        char ch = (*it).at(letterIndex - 1);
        if (!isUndoFunc())
            TextEditor::getUndo() -> submit(Undo::Action::DELETE, curRow, letterIndex - 1, ch);
        (*it).erase(letterIndex - 1, 1);
        letterIndex--;
        change(false);
        return;
    }
    if (curRow == 0) return;
    list<string>::iterator prev = it; prev--;
    letterIndex = (*prev).size();
    curRow--;
    maxRow--;
    if (!isUndoFunc())
        TextEditor::getUndo() -> submit(Undo::Action::JOIN, curRow, letterIndex);
    (*prev) += (*it);
    docU.erase(it);
    it = prev;
    change(false);
}

void StudentTextEditor::insert(char ch) {
    if (ch == '\t')
    {
        string s = "    ";
        (*it).insert(letterIndex, s);
        for (int i = 0; i < 4; i++)
        {
            letterIndex++;
            TextEditor::getUndo() -> submit(Undo::Action::INSERT, curRow, letterIndex, ' ');
        }
        return;
    }
    if (!isUndoFunc())
        TextEditor::getUndo() -> submit(Undo::Action::INSERT, curRow, letterIndex + 1);
    if (letterIndex == (*it).size())
    {
        (*it) += ch;
        letterIndex++;
        change(false);
        return;
    }
    string s = "";
    s += ch;
    change(false);
    (*it).insert(letterIndex, s);
    letterIndex++;
}

void StudentTextEditor::enter() {
    /*
    if (curRow == maxRow)
    {
        if (!isUndoFunc())
            TextEditor::getUndo() -> submit(Undo::Action::SPLIT, curRow, letterIndex);
        change(false);
        docU.push_back((*it).substr(letterIndex));
        (*it) = (*it).substr(0, letterIndex);
        curRow++;
        maxRow++;
        letterIndex = 0;
        it++;
        return;
    }
    */
    list<string>::iterator next = it;
    next++;
    if (!isUndoFunc())
        TextEditor::getUndo() -> submit(Undo::Action::SPLIT, curRow, letterIndex);
    change(false);
    docU.insert(next, (*it).substr(letterIndex));
    (*it) = (*it).substr(0, letterIndex);
    letterIndex = 0;
    it++;
    curRow++;
    maxRow++;
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = curRow;
    col = letterIndex;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const
{
    if (startRow < 0 || numRows < 0 || startRow > maxRow)
        return -1;
    int cur = curRow;
    int noLines = 0;
    list<string>::iterator iter = it;
    while (cur < startRow){ cur++; iter++; }
    while (cur > startRow){ cur--; iter--; }
    int index = 0;
    lines.clear();
    while (cur <= maxRow && noLines < numRows)
    {
        lines.push_back((*iter));
        index++;
        iter++;
        noLines++;
        cur++;
    }
    
    return noLines;
}

void StudentTextEditor::undo() {
    int counter;
    int origRow = curRow;
    string text;
    Undo::Action actor = TextEditor::getUndo() -> get(curRow, letterIndex, counter, text);
    while (origRow > curRow)
    {
        origRow--;
        it--;
    }
    while (origRow < curRow)
    {
        origRow++;
        it++;
    }
    if (actor == Undo::Action::INSERT)
    {
        for (int i = 0; i < text.size(); i++)
        {
            change(true);
            insert(text.at(i));
        }
    }
    if (actor == Undo::Action::DELETE)
    {
        for (int i = 0; i < counter; i++)
        {
            change(true);
            backspace();
        }
    }
    if (actor == Undo::Action::SPLIT)
    {
        change(true);
        enter();
    }
    if (actor == Undo::Action::JOIN)
    {
        change(true);
        del();
    }
}

