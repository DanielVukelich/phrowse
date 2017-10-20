# Phrowse
A terminal based gopher browser

### Building

### Requirements
*libncurses (or your system's equivalent package for ncurses headers)
*libubsan (only required for debugging, should be included with your compiler's libs package)

### Running
Just

    make release

and the binary will be built in the root folder

### Using
Launch phrowse with

    ./phrowse <url>

Navigate documents with by using the arrow keys UP and DOWN.  You can also use PAGEUP and PAGEDOWN to move an entire screen up/down.  Pressing 'i' will move the cursor up to the nearest selectable menu item, and pressing 'k' will move the cursor down to the nearest selectable item.  Select menu items with ENTER.  Pressing arrow keys LEFT or RIGHT will go backward and forward in your history respectively.  Exit Phrowse by pressing CTRL-C or CTRL-D at (almost) any time.  Pressing 'g' on a page will allow you to go to any valid url you type in.

### Debugging
Run

    make debug

You will need to have libubsan installed in order to compile with the sanitizers.  In the event that the address sanitizer catches an error, the log file will be written to `asan.log.$PID`.  If you want to attach a debugger, phrowse will display its PID in the bottom right corner.

### Notes
Not all gopher menu types are supported yet.  Only gopher menus, text documents, and search items are supported at the moment.  Trying to select an unsupported menu item may cause Phrowse to crash at the moment.

At the moment, document caching is permanent for the Phrowse session.  That means that if you try to visit any gopherspaces that use dynamic content, the first version of that document you receive will be cached until you close phrowse.
