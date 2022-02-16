#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

static string readFile(string path)
{
    try
    {
        ifstream t(path);
        if (t)
        {
            ostringstream buffer;
            buffer << t.rdbuf();
            return buffer.str();
        }
    }
    catch(exception e)
    {
        // throw error friendly error
        return "";

    }
}
