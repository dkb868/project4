#include <iostream>
#include "DiskMultiMap.h"
#include "IntelWeb.h""
using namespace std;

int main() {
    DiskMultiMap map;
    map.openExisting("wtf");
    map.insert("key","value","context");
    cout << "Hello, World!" << endl;
    IntelWeb intelWeb;
    cout << intelWeb.ingest("lol");
    return 0;
}