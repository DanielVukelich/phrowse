# Phrowse
A terminal based gopher browser

### Building
Make sure you have ncurses-dev (or your system's equivalent package) installed.  Then just

    make release
    
and the binary will be built in the root folder

### Using
Launch phrowse with

    ./phrowse <url>
    
Navigate documents with by using the arrow keys UP and DOWN.  Select menu items with ENTER.  Pressing arrow keys LEFT or RIGHT will go backward and forward in your history respectively.  Exit Phrowse by pressing CTRL-C or CTRL-D at (almost) any time.  Pressing 'g' on a page will allow you to go to any valid url you type in.

### Notes
Not all gopher menu types are supported yet.  Only gopher menus, text documents, and search items are supported at the moment.  Trying to select an unsupported menu item may cause Phrowse to crash at the moment.
