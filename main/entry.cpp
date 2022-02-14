#include "iostream"
#include "entry.h"
#include "tokenizer.cpp"
using namespace std;
using namespace flow;

bool cmd = true;
bool debug = true;
bool instantCompile = true;

static const string placeholder = "/**************************************************\nCOPYRIGHT (2022) OF FLOW INC.\n**************************************************/\n\n\n";
static std::map<string, string> language = {
    {"string", "\\w+"}

};
static std::map<string, string> flowtocpp = {
    {"auto", "method"},
    {"int", "number"},
    {"decimal", "number"},
    {"float", "number"},
    {"long", "number"},
    {"short", "number"},
    {"\n", ";\n"},
    {"finish","return"}
};
static std::map<string,string> flowkeyneedsLib = {
    {"string","#include <string>\n"}
};
string compileToCPP(lexertk::generator gen)
{
    std::set<string> usedincludes_usings;
    string includes_usings;

    string ret = placeholder + "using namespace std;";
    for (std::size_t i = 0; i < gen.size(); ++i)
    {
        lexertk::token t = gen[i];
        if (flowkeyneedsLib.find(t.value) != flowtocpp.end() && usedincludes_usings.find(t.value) == usedincludes_usings.end()){
            //we need an include
            includes_usings.append(flowkeyneedsLib.find(t.value)->second + " ");
            usedincludes_usings.insert(t.value);
        }
        if(flowtocpp.find(t.value) != flowtocpp.end()){
            //we have a predefined static token (see above)
            ret.append(flowtocpp.find(t.value)->second + " ");
        }else{
            ret.append(t.value + " ");
        }
        
    }
    includes_usings += "\n\n\n" + ret;
    return includes_usings;
}
bool compile(string fc, bool runCompileAfter = false)
{
    try
    {
        lexertk::generator result = Tokenizer::compile(fc);
        cout << "result:" << endl;
        if (result.size() <= 0)
        {
            // if no tokens were found.
            cout << "file was empty?";
            exit(1);
        }
        fileManager::writeToFile("./output.cpp",compileToCPP(result));

        return true;
    }
    catch (exception e)
    {
        return false;
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
