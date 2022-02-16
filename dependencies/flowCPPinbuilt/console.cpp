#include <string>
#include <iostream>

using namespace std;


static void write(auto a){
    cout << a << endl; 
}
static string question(string question){
    cout << question << endl;
    string res;

    cin >> res;
    return res;
}