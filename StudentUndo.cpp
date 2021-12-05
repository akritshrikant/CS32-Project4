#include "StudentUndo.h"
#include <cmath>

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    UndoInfo info = UndoInfo(action, row, col, ch);
    undoStack.push(info);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if (undoStack.empty()) return StudentUndo::Action::ERROR;
    StudentUndo::Action act;
    UndoInfo first = undoStack.top();
    undoStack.pop();
    row = first.m_row;
    col = first.m_col;
    count = 0;
    text = "";
    if (first.m_action == Undo::Action::SPLIT)
    {
        count = 1;
        return Undo::Action::JOIN;
    }
    if (first.m_action == Undo::Action::JOIN)
    {
        count = 1;
        row = first.m_row;
        col = first.m_col;
        return Undo::Action::SPLIT;
    }
    if (first.m_action == Undo::Action::INSERT)
    {
        act = Undo::Action::DELETE;
        row = first.m_row;
        col = first.m_col;
        count = 1;
        if (undoStack.empty()) return act;
        UndoInfo next = undoStack.top();
        while (first.m_action == next.m_action && next.m_col - first.m_col == -1 &&
               next.m_row == first.m_row)
        {
            count++;
            first = next;
            undoStack.pop();
            if (undoStack.empty()) return act;
            next = undoStack.top();
        }
        return act;
    }
    if (first.m_action == Undo::Action::DELETE)
    {
        act = Undo::Action::INSERT;
        bool isDel = false;
        count = 1;
        row = first.m_row;
        text += first.m_char;
        if (undoStack.empty()) return act;
        UndoInfo next = undoStack.top();
        int diff = next.m_col - first.m_col;
        if (next.m_action == first.m_action && diff == 0 && next.m_row == first.m_row)
        {
            isDel = true;
            while (next.m_action == first.m_action && diff == 0 && next.m_row == first.m_row)
            {
                text += next.m_char;
                first = next;
                undoStack.pop();
                if (undoStack.empty()) break;
                next = undoStack.top();
                diff = next.m_col - first.m_col;
            }
        }
        else if (next.m_action == first.m_action && diff == 1 && next.m_row == first.m_row)
        {
            while (next.m_action == first.m_action && diff == 1 && next.m_row == first.m_row)
            {
                text += next.m_char;
                first = next;
                undoStack.pop();
                if (undoStack.empty()) return act;
                next = undoStack.top();
                diff = next.m_col - first.m_col;
            }
        }
        if (isDel)
        {
            int n = text.size();
            for (int i = 0; i < n / 2; i++)
                std::swap(text[i], text[n - i - 1]);
        }
            
        return act;
    }
    return Undo::Action::ERROR;
}

void StudentUndo::clear() {
	while (!undoStack.empty())
        undoStack.pop();
}
