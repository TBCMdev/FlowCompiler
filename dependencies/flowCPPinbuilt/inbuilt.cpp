#include <string>
namespace flow
{
    /*READ
        this file is all the inbuilt structs that make up the flow language.
    */

    static void end(int PROC_CODE = 0){
        exit(PROC_CODE);
    }
    static void stop(int PROC_CODE = 0){
        exit(PROC_CODE);
    }
    static void quit(int PROC_CODE = 0){
        exit(PROC_CODE);
    }
    
    
    string operator+(string& s, int& i){
        return s + to_string(i);
    }
    struct coordinate{
        float x,y,z;

        public:
            coordinate(float _x,float _y, float _z){
                x = _x;
                y = _y;
                z = _z;
            }
    };
}