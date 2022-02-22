#include "iostream"
#include "entry.h"
#include "tokenizer.cpp"
#include "complex_parsing.cpp"
#include <vector>
#include <functional>
using namespace std;
using namespace flow;

bool cmd = true;
bool debug = true;
bool instantCompile = true;
bool format = true; // turn false for file content to be less

static const string placeholder = "/**************************************************\nCOPYRIGHT (2022) OF FLOW INC.\n**************************************************/\n\n\n";
static std::set<string> nonliners = {"{" /*"}" REMOVED BECAUSE OF ARRAY IMPLEMENTATION*/};

static std::map<string, std::function<string(lexertk::generator, int)>> typeCheckers = {
    {"thing", _checkType},
    {"var", _checkType},
};
static std::map<string, string> flowtocpp = {
    {"string", "std::string"},
    {"method", "auto"},
    {"number", "float"},
    {"finish", "return"},
    {"extra", "#include "},
    {"start", "int main"},
    {"Start", "int main"},
    {"START", "int main"},
    {"var", "auto"},
    {"thing", "auto"},
    {"loop", "for"},
    {"forever", "while(true)"},
    // OPERATORS
    {"or", "||"},
    {"and", "&&"},
    {"is", "=="},
    {"equals", "=="},
    {"in", ":"}};
static std::set<string> FLOWCPPDEPENDENCIES = fileManager::readflowcppdep(true);
static std::map<string, string> flowkeyneedsLib = {
    //{"string", "#include <string>\n"}
};

string compileToCPP(lexertk::generator gen);
bool compile(string fc, string output, bool runCompileAfter = false)
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
        fileManager::writeToFile(output, res);
        // Formatter::formatFile(3,(**char)"");
        system("C:\\msys64\\mingw64\\bin\\g++.exe .\\output.cpp -o output.exe");
        return true;
    }
    catch (logic_error &e)
    {
        cout << "caught:" << e.what() << endl;
        return false;
    }
}
string compileToCPP(lexertk::generator gen)
{
    bool usemain = false;
    bool inmainloop = false;
    int SKIPC = 0;
    bool parsingimport = false;
    std::set<string> usedincludes_usings;
    string includes_usings = "";
    string main = "";
    int OPEN_BR = 0; // when in main loop, we must check until we fully exit ALL statements with } before exiting int main () writing exec.

    for (auto x : FLOWCPPDEPENDENCIES)
    {
        if (x == "" || x == " ")
            continue;
        includes_usings.append("#include \"" + x + "\"\n");
    }

    string ret = placeholder;
    // MAIN PARSING LOOP (INT skip is used for complex parsing. such as for loops.)
    for (std::size_t i = 0; i < gen.size(); ++i)
    {

        if (SKIPC > 0)
        {
            SKIPC--;
            continue;
        }

        lexertk::token t = gen[i];
        string first = flowtocpp.find(t.value) == flowtocpp.end() ? "" : flowtocpp.find(t.value)->first;
        string second = flowtocpp.find(t.value) == flowtocpp.end() ? "" : flowtocpp.find(t.value)->second;
        cout << "compiling..." << t.value << endl;
        if (first == "loop")
        {
            cout << "calling" << endl;
            auto a = parseLoop(flowtocpp, gen, i + 1);
            cout << "test:" << get<0>(a) << endl
                 << get<1>(a) << endl
                 << get<2>(a) << endl;
            if (!inmainloop)
            {
                ret.append("for ");
                ret.append(get<0>(a));
            }
            else
            {
                main.append("for ");
                main.append(get<0>(a));
            }

            SKIPC = get<1>(a);
            OPEN_BR += get<2>(a);
            continue;
        }
        // TODO ABOVE!!
        if (parsingimport)
        {

            if (first == "from")
            {
                // erase from as it is a formality keyword and continue
                continue;
            }
            else if (t.value.front() == '"')
            {
                // expected str:
                t.value.append("\n");
                // finally, parse the file
                string filename = fileManager::ReplaceAll(t.value, "\"", "");
                cout << "opening:" << filename << endl;
                string cont = fileManager::readFileIntoString(filename, nonliners);
                compile(cont, filename, false);
                parsingimport = false;
            }
            else
            {
                continue;
            }
        }

        if (first == "start" ||
            first == "Start" ||
            first == "START")
        {
            inmainloop = true;
            usemain = true;
        }
        if (first == "extra")
        {
            parsingimport = true;
        }
        if (flowkeyneedsLib.find(t.value) != flowkeyneedsLib.end() && usedincludes_usings.find(t.value) == usedincludes_usings.end())
        {
            // we need an include
            includes_usings.append(second + " ");
            usedincludes_usings.insert(t.value);
        }

        if (flowtocpp.find(t.value) != flowtocpp.end())
        {
            // we have a predefined static token (see above)
            if (typeCheckers.find(t.value) == typeCheckers.end())
            {
                if (!inmainloop)
                    ret.append(second + " ");
                else
                    main.append(second + " ");
            }else{
                cout << "found type checker keyword!" << endl;
                if (!inmainloop)
                    ret.append(typeCheckers.find(t.value)->second(gen,i) + " ");
                else
                    main.append(typeCheckers.find(t.value)->second(gen,i) + " ");
            }
        }
        else
        {
            if (!inmainloop)
                ret.append(t.value);
            else
                main.append(t.value);
        }
        if (inmainloop && t.value == "{")
        {
            OPEN_BR++;
        }
        if (inmainloop && t.value == "}")
        {
            OPEN_BR--;
            if (OPEN_BR <= 0)
                inmainloop = false;
        }
    }
    cout << "main:" << main << endl;
    includes_usings += "\n\n\n" + ret + "\n\n" + main;
    return includes_usings;
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
                compile(cont, "./output.cpp", true);
            }
        }
        else
        {
            // instant compile
            compile(cont, "./output.cpp");
        }
    }
    return 0;
}
