#include "iostream"
#include "entry.h"
#include "tokenizer.cpp"
using namespace std;
using namespace flow;
bool cmd = true;
bool debug = true;
bool instantCompile = true;
bool format = true; // turn false for file content to be less
static const string placeholder = "/**************************************************\nCOPYRIGHT (2022) OF FLOW INC.\n**************************************************/\n\n\n";
static std::map<string, string> language = {
    {"string", "\\w+"}

};
static std::set<string> nonliners = {"{","}"};
static std::map<string, string> flowtocpp = {
    {"method", "auto"},
    {"number", "float"},
    {"finish", "return"},
    {"start", "int main"},
    {"Start", "int main"},
    {"START", "int main"}
    };
static std::set<string> FLOWCPPDEPENDENCIES = fileManager::readflowcppdep();
static std::map<string, string> flowkeyneedsLib = {
    {"string", "#include <string>\n"}};
string compileToCPP(lexertk::generator gen)
{
    bool usemain = false;
    bool inmainloop = false;

    std::set<string> usedincludes_usings;
    string includes_usings;
    string main;
    for(auto x : FLOWCPPDEPENDENCIES){
        includes_usings.append("#include \"" + x + "\"\n");
    }

    string ret = placeholder + "using namespace std;\n\n";
    for (std::size_t i = 0; i < gen.size(); ++i)
    {


        lexertk::token t = gen[i];

        if (flowtocpp.find(t.value)->first == "start" ||
            flowtocpp.find(t.value)->first == "Start" ||
            flowtocpp.find(t.value)->first == "START"){
                inmainloop = true;
                usemain = true;
            }
        if (flowkeyneedsLib.find(t.value) != flowkeyneedsLib.end() && usedincludes_usings.find(t.value) == usedincludes_usings.end())
        {
            // we need an include
            includes_usings.append(flowkeyneedsLib.find(t.value)->second + " ");
            usedincludes_usings.insert(t.value);
        }
        
        if (flowtocpp.find(t.value) != flowtocpp.end())
        {
            // we have a predefined static token (see above)
            if (!inmainloop) ret.append(flowtocpp.find(t.value)->second + " ");
            else if (flowtocpp.find(t.value)->first == "start") main.append(flowtocpp.find(t.value)->second + " ");
        }
        else
        {
            if(!inmainloop) ret.append(t.value + " ");
            else main.append(t.value + " ");
        }
        if(inmainloop && t.value == "}"){
            inmainloop = false;
        }

    }
    cout << "main:" << main << endl;
    includes_usings += "\n\n\n" + ret + "\n\n" + main;
    return includes_usings;
}
bool compile(string fc, bool runCompileAfter = false)
{
    try
    {
        lexertk::generator result = Tokenizer::compile(fc);
        // cout << "result:" << endl;
        if (result.size() <= 0)
        {
            // if no tokens were found.
            cout << "file was empty?";
            exit(1);
        }
        string res = compileToCPP(result);
        fileManager::writeToFile("./output.cpp", res);
        //Formatter::formatFile(3,(**char)"");
        system("C:\\msys64\\mingw64\\bin\\g++.exe .\\output.cpp -o output.exe");
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
        

        string cont = fileManager::readFileIntoString(path, nonliners);
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
