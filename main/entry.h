#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <experimental/filesystem>


#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem::v1;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif


using namespace std;


//USED TO STOP FILESYSTEM NAMESPACE ERROR
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
        inline static std::set<string> readflowcppdep(){
            std::set<string> res;
            for (const auto & file : fs::directory_iterator("C:\\Flow\\bin\\")){
                res.insert("C:\\Flow\\bin\\" + file.path().filename().string());
            }

            return res;
        }
        static bool has_suffix(const std::string &str, const std::string &suffix)
        {
            return str.size() >= suffix.size() &&
                   str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }
        inline static std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
        {
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
            }
            return str;
        }
        inline static string readFileIntoString(string path, std::set<string> s)
        {
            try
            {
                ifstream t(path);
                if (t)
                {
                    ostringstream buffer;
                    buffer << t.rdbuf();
                    string str = buffer.str();

                    // cout << "returning:" + ReplaceAll(str,"\n","\\n") << endl;

                    return endliner(str, s);
                }

                return "";
            }
            catch (exception e)
            {
                cout << "an unexpected error has occured while trying to open the file." << endl;
                return "";
            }
        }
        inline static string endliner(string prev, std::set<string> s)
        {
            istringstream f(prev);
            string line;
            string finished;
            while (getline(f, line))
            {
                cout << "line:" + line << endl;

                bool needs = false;
                for (auto a : s)
                {
                    if (!has_suffix(line,a) && !line.empty() && line.find_first_not_of(' ') != std::string::npos)//no endliner exception, line isnt empty, and it is not fully whitespaces.
                        needs = true;
                    else
                        needs = false;
                }
                cout << needs << endl;
                if (needs){
                    cout << "appending" + (line + ";") << endl;
                    finished.append(line + ";");
                }else{
                    finished.append(line);
                }
            }
            return finished;
        }
        inline static bool writeToFile(string path, string cont, string placeholder = "")
        {
            try
            {
                ofstream s(path);

                s << placeholder + cont;

                s.close();

                return true;
            }
            catch (exception e)
            {
                return false;
            }
        }
    };
}