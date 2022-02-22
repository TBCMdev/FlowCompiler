#include <string>
#include <iostream>

using namespace std;

static void write(auto a){
    cout << a << endl; 
}
static void write(auto a,bool endline){
    cout << a << endline ? endl : ""; 
}
template <typename T>
void _write_T(T t){
    cout << t << endl;
}

template<typename T,typename T1, typename... Args>
static void write(T a, T1 t, Args... args){
    cout << a;
    _write_T(t);
    _write_T(args...);
}

template<typename T,typename T1>
static void write(T a, T1 t,bool endline = false){
    cout << a << endline ? endl : "";
    _write_T(t);
}
static string question(string question){
    cout << question;
    string res; 

    cin >> res;
    return res;
}