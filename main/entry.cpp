#include "iostream"
#include "entry.h"
#include "tokenizer.cpp"
using namespace std;
using namespace flow;

bool cmd = true;
bool debug = true;
bool instantCompile = true;

std::map<string, string> language = {
    {"string", "\\w+"}

};

bool compile(string fc, bool runCompileAfter = false)
{
    try
    {
        Tokenizer t(language);
        std::list<string> result = t.compile(fc);
        cout << "result:" << endl;
        if (result.size() <= 0){
            //if no tokens were found.
            cout << "file was empty?";
            exit(1);
        }
        cout << "results below:" << endl;
        for (const string &x : result)
        {
            //looping through found tokens.
            cout << x << ": " << x << "\n";
        }
        return 1;
    }
    catch (exception e)
    {
        return 0;
    }
}
int main()
{
    if (cmd)
    {
        string path = getUserIN("compile file path: ");
        if (path == "def")
            path = "./test.flo";
        if (debug)
            cout << "opening \"" + path + "\" ..." << endl;
        string cont = fileManager::readFileIntoString(path);
        if (cont.empty())
        {
            cout << "File does not exist.";
            main();
        }
        if (debug)
            cout << "reading: \n\n" + cont << endl;

        if (!instantCompile)
        {
            if (tolower((getUserIN("run after compile? (Y/N): ")) == "y"))
            {
                // TODO: run file after compiling is done.
                compile(cont, true);
            }
        }
        else
        {
            // instant compile
            compile(cont);
        }
    }
    return 0;
}
