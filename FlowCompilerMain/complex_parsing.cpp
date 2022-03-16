#include <iostream>
#include "tokenizer.cpp"
using namespace std;
bool is_number(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c)
                                      { return !std::isdigit(c); }) == s.end();
}
static bool is_op(const string &s)
{
    return !s.empty() && (s == "<" || s == ">" || s == "<=" || s == ">=" || s == "!=" || s == "==");
}
static bool is_bool(const string &s)
{
    return !s.empty() && (s == "true" || s == "false");
}
static tuple<string, int, int> parseLoop(std::map<string, string> flowtocpp, lexertk::generator gen, int i)
{
    // RETURS APPENDER AND SKIPC AMOUNT

    string s = "";
    int c;
    int BRC = 1; // bracket count;
    lexertk::token t = gen[i];
    bool c_p1 = (i + 1 < gen.size());
    bool c_p2 = (i + 2 < gen.size());
    bool c_p3 = (i + 3 < gen.size());
    bool c_p4 = (i + 4 < gen.size());
    bool c_p5 = (i + 5 < gen.size());
    auto f1 = c_p1 ? gen[i + 1].value : "";
    auto f2 = c_p2 ? gen[i + 2].value : "";
    if (debug)
        cout << "test:" << t.value << endl
             << f1 << endl
             << f2 << endl
             << gen[i + 3].value << endl
             << gen[i + 4].value << endl
             << gen[i + 5].value << endl;
    /*
               loop([int, bool, var] i, [inc | increase | dec | decrease] [val])

               ex:
                   loop(number myIncLoopNumber, increase 5){

                   } or

                   loop(thing myThing in myListofThings){
                       write(myThing)
                   } or

                   loop(10){

                   } or
                   loop(true){

                   } or
                   forever{

                   }
           */

    // t.val = loop
    // f1 = (
    // f2 = [int, bool, var] (i) | number
    string conv = "";

    if (t.value == "(")
    {
        s.append("(");
        c++;
    };

    if (t.value == "{")
        BRC++;
    else if (t.value == "}")
        BRC--;
    else
        conv = t.value;

    if (c_p2)
    {
        if (f1 == "number")
        {
            /*loop(number myIncLoopNumber, increase 5){} |OR| loop(thing myThing in myListofThings){}*/
            // f2 is var name
            // f3 should be ,
            // f4 should be [inc,dec,increase,decrease]
            // f5 should be change val
            bool condition = false;
            string _c;  // condition: "< 20"
            string R_c; // RAW condition: "20"
            if (c_p3 && is_op(gen[i + 3].value))
            {
                condition = true;
                _c = gen[i + 3].value + gen[i + 4].value;
                R_c = gen[i + 4].value;
                if (debug)
                    cout << "found cond:" + _c << endl;
                i += 2;
                c += 2;
            }
            if (c_p3 && gen[i + 3].value == ",")
            {
                if (debug)
                    cout << gen[i + 3].value << "is comma" << endl;
                /*loop(number myIncLoopNumber, increase 5){}*/
                if (c_p4)
                {
                    if (c_p5 && !is_number(gen[i + 5].value)) // if we dont have a value to change by
                    {
                        if (debug)
                            cout << "doesnt have number to change by" << endl;
                        string type = gen[i + 4].value;
                        if (type == "increase" || type == "inc")
                        {
                            conv = "float " + f2 + " = 0; " + f2 + (condition ? _c : "<=" + to_string(INT_MAX)) + ";" + f2 + "++";
                        }
                        else if (type == "decrease" || type == "dec")
                        {

                            conv = "float " + f2 + " = 0; " + f2 + (condition ? _c : ">=" + to_string(INT_MAX)) + ";" + f2 + "--";
                        }
                        int CONDITION_TRUE_EX = (condition ? 3 : 0);
                        c = 4 + CONDITION_TRUE_EX;
                        /*else
                        {
                            // probably standard operator:
                            conv = "float " + f2 + " = 0; " + f2 + ">" + to_string(INT_MAX) + ";" + type;
                        }*/
                    }
                    else if (c_p5 && is_number(gen[i + 5].value))
                    { // if we do have a value to change by
                        if (debug)
                            cout << "has number to change by" << endl;
                        string type = gen[i + 4].value;
                        if (type == "increase" || type == "inc")
                        {
                            conv = "float " + f2 + " = 0; " + f2 + (condition ? _c : "<=" + gen[i + 5].value) + ";" + f2 + "+= " + gen[i + 5].value;
                        }
                        else if (type == "decrease" || type == "dec")
                        {

                            conv = "float " + f2 + " = 0; " + f2 + (condition ? _c : ">=" + gen[i + 5].value) + ";" + f2 + "-=" + gen[i + 5].value;
                        }
                        /*else
                        {
                            // probably standard operator:
                            conv = "float " + f2 + " = 0; " + f2 + ">" + to_string(INT_MAX) + ";" + type;
                        }*/
                        int CONDITION_TRUE_EX = (condition ? 3 : 1);
                        c = 5 + CONDITION_TRUE_EX;
                    }
                }
            }
        }
        else if (c_p4 && gen[i + 3].value == "in")
        {
            /*loop(thing myThing in myListofThings){}*/
            conv = "auto " + gen[i + 2].value + " : " + gen[i + 4].value;
            c = 5;
        }
        else if (is_number(f1))
        {
            if (debug)
                cout << f1 << "is number" << endl;
            conv = "int ___ = 0; ___ < " + f1 + "; ___++ "; // add traditional for loop with untracable int name
            c = 2;
        }
        else if (is_bool(f1))
        {
            if (debug)
                cout << f1 << "is bool" << endl;
            conv = ";;";
            c = 2;
        }
    }
    if (debug)
        cout << "appending" << conv << endl;
    s.append(conv); // we will just append if nothing is found

    tuple<string, int, int> res(s, c, BRC);
    return res;
}

static string _checkType(lexertk::generator gen, int i)
{
    lexertk::token orig = gen[i];
    // gen[i + 1].val == token name
    // gen[i + 2].val == "="
    // gen[i + 3].val == start of VALUE
    try
    {
        lexertk::token val = gen[i + 3];
        if (debug)
            cout << "val:" << val.value << endl;
        if (val.value == "{")
        {
            // treat like std::vector<type> and get type
            for (int x = i + 3; x < gen.size(); x++)
            {
                if (gen[x].value == "\"" || gen[x].value == "'")
                {
                    // we know its a string array, which is the only occurance in my testing that causes an error with vectors and arrays.
                    return "std::vector<string>";
                }
            }
        }
        // ADD MORE DEFINABLE TYPES IF NEED BE (if auto keyword does not cover.)
        // then add the keyword and this func to entry.cpp::typeCheckers
        return "auto";
    }
    catch (exception ignore)
    {
        return orig.value;
    }
}
static void CompileIncludedFile(string &path)
{
    try
    {
        fs::path p(path);
        p.replace_extension(".cpp");
        compile(fileManager::readFileIntoString(path, nonliners), inc_path + p.filename().string(), false, "", false);
    }
    catch (exception e)
    {
    }
}
static std::tuple<string, int> parseExtraStatement(lexertk::generator gen, int i)
{
    lexertk::token orig = gen[i];

    /*
        SYNTAX: extra "[file name]"
        gen[i] = "extra"
        gen[i + 1] = VAL

    */
    try
    {

        if (i + 1 > gen.size())
            return tuple<string, int>(orig.value, 0);
        lexertk::token val = gen[i + 1];
        if (val.value.front() == '"')
        {
            val.value.erase(0, 1);                 // erase the first character
            val.value.erase(val.value.size() - 1); // erase the last character
        }
        if (debug)
            cout << "finished:" << val.value << endl;
        fs::path p(val.value);
        if (!fs::is_directory(p))
        {
            CompileIncludedFile(val.value);
            p.replace_extension(".cpp");

            return tuple<string, int>("#include \"./includes/" + p.filename().string() + "\"", 1);
        }
        else
        {
            fs::copy(p, fs::path(inc_path));
            return tuple<string, int>("", 0);
        }
    }
    catch (exception ignored)
    {
        if (debug)
            cout << "exception:" << ignored.what() << endl;
        return tuple<string, int>(orig.value, 0);
    }
}

static std::tuple<string, int> parseModuleStatement(lexertk::generator gen, int i)
{
    /*
        from: "module [module name]"
        to: "using namespace [module name]"
    */
    if (i + 1 > gen.size())
        return tuple<string, int>("", 0);
    cout << "finding:" << gen[i + 1].value << endl;
    if (gen[i + 1].value == "Game")
        USING_GAME_LIB = true;
    return tuple<string, int>("using namespace " + gen[i + 1].value + ";", 1);
}
[[deprecated("sticking to normal class dec")]]
static std::tuple<string, int> parseClassDeclaration(lexertk::generator gen, int i)
{
    if(debug) cout << "PARSING CLASS" << endl;
    /*
        thing myclass = new MyClass()
        converts to:
        MyClass myclass([params]);
        gen[i] = "new"
        gen[i + 1] = class name
        gen[i - 2] = varname
        gen[i - 3] = type dec: thing, var

        by the time we reach this function, we already have parsed thing or var to: "auto", and the var name and the
        = symbol. we are at the index of the "new" string.
    */
    lexertk::token t = gen[i];
    if (i + 1 > gen.size())
        return tuple<string, int>(t.value, 0);
    string __ret = gen[i + 1].value;
    int __count = 1;
    for (std::size_t _i = i + 2; _i < gen.size(); ++_i)
    {
        __ret.append(gen[_i].value);
        __count++;
        if (gen[_i].value == ")")
            break;
    }
    if (debug) cout << "class declaration:" << __ret << endl;

    return tuple<string, int>(__ret.append(";"), __count);
}
bool isFloatExt(string s){//second is number func for floats 
    for(string::size_type x = 0; x < s.size(); x++){
        //if (lexertk::details::is_digit(s[x])){}
        if(s[x] == '.'){
            if(x + 1 > s.length()) return false;
            if(lexertk::details::is_digit(s[x + 1])) return true;
        }
    }
    return false;

}
static bool isFloat(lexertk::generator gen, int i){


    if(isFloatExt(gen[i].value)) return true;


    return false;
}