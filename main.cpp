#include <iostream>
#include "DiskMultiMap.h"

using namespace std;

int main() {
    DiskMultiMap map;
    map.openExisting("wtf");
    map.insert("key","value","context");
    cout << "Hello, World!" << endl;
    return 0;
}