#include "includes.h"

class CDB {
    public:
        bool busy;
        int value;

    CDB(){
        busy = false;
        value = 0;
    }
    void set_value(int Result) {
        value = Result;
        busy = true;
    }
};