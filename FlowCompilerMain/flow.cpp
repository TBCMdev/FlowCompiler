#include "iostream"
#include "entry.h"
#include "tokenizer.cpp"
#include "complex_parsing.cpp"
#include "InputParser.cpp"
#include <iomanip>
#include <ctime>
#include <vector>
#include <functional>
using namespace std;
using namespace flow;

static const string placeholder = "/**************************************************\nCOPYRIGHT (2022) OF FLOW INC.\n**************************************************/\n\n\n";

static std::map<string, std::function<string(lexertk::generator, int)>> typeCheckers = {
    {"thing", _checkType},
    {"var", _checkType},
};
static std::map<string, std::function<std::tuple<string, int>(lexertk::generator, int)>> uniqueParsers = {
    {"extra", parseExtraStatement},
    {"module", parseModuleStatement}
    //{"new",parseClassDeclaration}
};
static std::map<string, string> flowtocpp = {
    {"string", "std::string"},
    {"method", "auto"},
    {"number", "float"},
    {"finish", "return"},
    {"extra", "#include "},
    {"module", "using namespace "},
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
static std::set<string> FLOWCPPDEPENDENCIES = fileManager::readflowcppdep(false);
static std::map<string, string> flowkeyneedsLib = {
    //{"string", "#include <string>\n"}
};

string compileToCPP(lexertk::generator gen);
bool flow::compile(string fc, string outputcpp, bool outputex = true, string outputexe = "", bool runCompileAfter = false)
{
    try
    {
        lexertk::generator result = Tokenizer::compile(fc);
        // if (debug) cout << "result:" << endl;
        if (result.size() <= 0)
        {
            // if no tokens were found.
            if (debug)
                cout << "file was empty?";
            exit(1);
        }
        string res = compileToCPP(result);
        fileManager::writeToFile(outputcpp, res);
        if (debug)
            cout << outputcpp << endl;
        // Formatter::formatFile(3,(**char)"");
        if (!USING_GAME_LIB)
        {
            cout << "-----NOT USING SFML-----" << endl;
            string cmd = "C:\\msys64\\mingw64\\bin\\g++.exe " + outputcpp + " -o " + outputexe;
            if (debug)
                cout << cmd << endl;
            if (outputex)
                system(cmd.c_str());
            return true;
        }
        else
        {
            cout << "-----USING SFML-----" << endl;
            fs::path path(outputcpp);
            fs::path noex(path);
            path.replace_extension(".o");
            noex.replace_extension("");
            string cmd = "g++ -c " + outputcpp + " -o " + path.string() + " -I\"C:\\Flow\\SFML-2.5.1\\include\" -DSFML_STATIC";
            string finish_cmd = "g++ " + path.string() + " -o " + outputexe + " -L\"C:\\Flow\\SFML-2.5.1\\lib\" -I \"C:\\Flow\\SFML-2.5.1\\include\" -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows -mconsole";
            if (debug)
                cout << cmd << endl;
            if (outputex)
            {
                system(cmd.c_str());
                system(finish_cmd.c_str());
            }
            return true;
        }
    }
    catch (logic_error &e)
    {
        if (debug)
            cout << "caught:" << e.what() << endl;
        return false;
    }
}
string compileToCPP(lexertk::generator gen)
{
    bool usemain = false;
    bool inmainloop = false;
    int SKIPC = 0;
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
        if (debug)
            cout << "compiling..." << t.value << endl;
        if (first == "loop")
        {
            if (debug)
                cout << "calling" << endl;
            auto a = parseLoop(flowtocpp, gen, i + 1);
            if (debug)
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
        if (first == "start" ||
            first == "Start" ||
            first == "START")
        {
            inmainloop = true;
            usemain = true;
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
            if (typeCheckers.find(t.value) != typeCheckers.end())
            {
                if (!inmainloop)
                    ret.append(typeCheckers.find(t.value)->second(gen, i) + " ");
                else
                    main.append(typeCheckers.find(t.value)->second(gen, i) + " ");
            }
            else if (uniqueParsers.find(t.value) != uniqueParsers.end())
            {
                std::tuple<string, int> t1 = uniqueParsers.find(t.value)->second(gen, i);
                if (debug)
                    cout << "found:" << get<0>(t1) << endl
                         << get<1>(t1) << endl;
                if (!inmainloop)
                    ret.append(get<0>(t1) + " ");
                else
                    main.append(get<0>(t1) + " ");
                SKIPC += get<1>(t1);
            }
            else
            {
                if (i - 1 > 0 && gen[i - 1].value == ".")
                {
                    if (!inmainloop)
                        ret.append(gen[i].value);
                    else
                        main.append(gen[i].value);
                }else{
                    if (!inmainloop)
                        ret.append(second + " ");
                    else
                        main.append(second + " ");
                }
            }
        }
        else
        {
            if (debug)
                cout << "has not found:" + t.value << endl;

            if(isFloat(gen,i)) {
                t.value += "f";
                //cout << "adding f to " << gen[i].value << ": " + t.value << endl;
            }
            if (!inmainloop)
                ret.append((!lexertk::details::is_invalid(t.value.c_str()[0]) && !lexertk::details::is_bracket(t.value.c_str()[0]) && t.value != "." ) ? t.value + " " : t.value);
            else
                main.append(
                    (
                        (!lexertk::details::is_invalid(t.value.c_str()[0]) && !lexertk::details::is_bracket(t.value.c_str()[0]))
                        && t.value != "."
                    ) ? t.value + " " : t.value);
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
    if (debug)
    {
        cout << "main:" << main << endl;
    }
    includes_usings += "\n\n\n" + ret + "\n\n" + main;
    return includes_usings;
}

int main(int argc, char **argv)
{
    bool runAfterCompile = false;
    bool format = false; // turn false for file content to be less
    InputParser p(argc, argv);
    if (p.cmdOptionExists("-d"))
        debug = true;
    else
        debug = false;
    if (p.cmdOptionExists("-fo"))
        format = true;
    if (p.cmdOptionExists("-r"))
        runAfterCompile = true;

    string path = argv[1];
    if (path.empty())
        return 1;
    string output = p.getCmdOption("-o");
    if (output.empty())
        output = "./output.exe";

    string cont = fileManager::readFileIntoString(path, nonliners);
    if (cont.empty())
    {
        if (debug)
            cout << "File does not exist.";
        return 1;
    }
    if (debug)
        cout << "reading: \n\n" + cont << endl;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    string dir_path = "C:\\Flow\\tasks\\" + oss.str();
    inc_path = dir_path + "\\includes\\"; // flow::inc_path
    string temp_path = dir_path + "\\main.cpp";
    fs::create_directory(fs::path{dir_path});
    fs::create_directory(fs::path{inc_path});
    ofstream{temp_path};
    compile(cont, temp_path, true, output, runAfterCompile);
    if (!p.cmdOptionExists("-s"))
    {
        remove(temp_path.c_str());
        fs::remove_all(dir_path);
        fs::remove(dir_path);
    }
    fs::path _(output);
    if (runAfterCompile)
        system(_.filename().string().c_str());
    return 0;
}
