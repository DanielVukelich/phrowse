#ifndef _HISTORY_H
#define _HISTORY_H

#include "menu.h"

#include <stack>
#include <utility>

class HistoryItem{
    friend class BrowserHistory;
public:
    HistoryItem();
    HistoryItem(MenuItem hist);
    HistoryItem(MenuItem hist, std::pair<unsigned int, unsigned int> indices);
    std::pair<unsigned int, unsigned int> get_indices();
    void set_indices(std::pair<unsigned int, unsigned int> indices);
    MenuItem& get_item();
private:
    MenuItem item;
    std::pair<unsigned int, unsigned int> hist_indices;
};

class BrowserHistory{
public:
    BrowserHistory();
    HistoryItem go_back();
    HistoryItem go_forward();
    void clear_future();
    void add_item(MenuItem item);
    void add_item(HistoryItem item);
    void set_hist_indices(std::pair<unsigned int, unsigned int> indices);
    void set_fut_indices(std::pair<unsigned int, unsigned int> indices);
private:
    std::stack<HistoryItem> history;
    std::stack<HistoryItem> future;
    bool last_item_was_fut = false;
};

#endif
