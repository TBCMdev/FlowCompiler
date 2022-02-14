#include "iostream"
#include "entry.h"

using namespace std;
using namespace flow;



bool cmd = true;
bool debug = true;
int main(){
    if (cmd){
        string path = getUserIN("compile file path: ");
        if (path == "def") path = "./test.flo";
        if (debug) cout << "opening \"" + path + "\" ..." << endl;
        string cont = fileManager::readFileIntoString(path);
        if (cont.empty()){
            cout << "File does not exist.";
            main();
        }
        if (debug) cout << "reading: \n\n" + cont << endl;
    }
    return 0;
}
