#include "history.h"

using std::stack;
using std::pair;
using std::make_pair;

HistoryItem::HistoryItem(){
    item = Menu::no_item;
    hist_indices = make_pair(0, 0);
}

HistoryItem::HistoryItem(MenuItem hist){
    item = hist;
    hist_indices = make_pair(0, 0);
}

HistoryItem::HistoryItem(MenuItem hist, pair<unsigned int, unsigned int> indices){
    item = hist;
    hist_indices = indices;
}

pair<unsigned int, unsigned int> HistoryItem::get_indices(){
    return hist_indices;
}

MenuItem HistoryItem::get_item(){
    return item;
}

void HistoryItem::set_indices(std::pair<unsigned int, unsigned int> indices){
    hist_indices = indices;
}

HistoryItem BrowserHistory::go_back(){
    if(history.size() < 2){
        return HistoryItem(Menu::no_item);
    }
    last_item_was_fut = false;
    HistoryItem temp = history.top();
    history.pop();
    future.push(temp);
    HistoryItem to_return = history.top();
    history.pop();
    return to_return;
}

void BrowserHistory::set_hist_indices(pair<unsigned int, unsigned int> indices){
    if(history.size()){
        history.top().set_indices(indices);
    }
}

HistoryItem BrowserHistory::go_forward(){
    if(!future.size()){
        return HistoryItem(Menu::no_item);
    }
    last_item_was_fut = true;
    HistoryItem to_return = future.top();
    future.pop();
    history.push(to_return);
    return to_return;
}

void BrowserHistory::clear_future(){
    last_item_was_fut = false;
    future = stack<HistoryItem>();
}

void BrowserHistory::add_item(MenuItem item){
    add_item(HistoryItem(item));
}

void BrowserHistory::add_item(HistoryItem item){
    if(!last_item_was_fut){
        history.push(item);
    }
}

BrowserHistory::BrowserHistory(){
    history = stack<HistoryItem>();
    future = stack<HistoryItem>();
}
