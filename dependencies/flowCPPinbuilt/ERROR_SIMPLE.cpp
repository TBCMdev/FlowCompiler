#include "iostream"
#include <string>
#include <map>
using namespace std;



class Error
{
private:
    string cont;

public:
    Error(string _cont, bool _throw = false)
    {
        if (_throw)
            throw _cont;
        cont = _cont;
    }
    Error()
    {

    }
    void thr()
    {
        throw cont;
    }
};

static const std::map<int,Error*> EASYERRORCNV = {
        {1,new Error()}
};

static void checkFileError(string f)
{

}