#include <iostream>
#include <map>
using namespace std;
int main(){
    std::map<string,string> m = {{"test","test1"}};

    for(auto _ : m){
        cout << _.first << endl;
    }
}