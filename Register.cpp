#include "includes.h"

class Register {
    public:
        bool busy;
        int Qi;

    Register(){
        busy = false;
        Qi = 0;
    }
    void set_Q(int Q) {
        Qi = Q;
        busy = true;
    }
};