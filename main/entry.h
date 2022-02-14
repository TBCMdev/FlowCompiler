#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
namespace flow
{
    string getUserIN(string s = "")
    {
        cout << s;
        string str;
        cin >> str;
        return str;
    }
    class fileManager
    {
    public:
        static string readFileIntoString(string path)
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

                return "";
            }
            catch (exception e)
            {
                cout << "an unexpected error has occured while trying to open the program." << endl;
                return "";
            }
        }
    };
}