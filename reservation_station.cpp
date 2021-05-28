#include "includes.h"

class reservation_station {
    public:
        string name;
        bool busy;
        string Op;
        int Vj, Vk, Qj, Qk, A;
        int PC;
        int issue_lat;
        int execute_lat;
        string inst;
        int instr_num;
    
    reservation_station() {
        name = "";
        busy = false;
        Op = "";
        Vj = 0;
        Vk = 0;
        Qj = 1;
        Qk = 1;
        A = 0;
        PC = 0;
        issue_lat = 0;
        execute_lat = 0;
        inst = "";
        instr_num = -1;
    }

    reservation_station(string Name, int Issue_Lat, int Execute_Lat) {
        name = Name;
        busy = false;
        Op = "";
        Vj = 0;
        Vk = 0;
        Qj = 1;
        Qk = 1;
        A = 0;
        PC = 0;
        issue_lat = Issue_Lat;
        execute_lat = Execute_Lat;
        inst = "";
        instr_num = -1;
    }



    bool operator <(const  reservation_station& b) const
    {
        return instr_num < b.instr_num;
    }
};

/*
class lw_rs : public reservation_station {
    lw_rs (string Name, int Issue_Lat, int Execute_Lat) : reservation_station( Name,  Issue_Lat,  Execute_Lat) {}
};

class sw_rs : public reservation_station {
    sw_rs (string Name, int Issue_Lat, int Execute_Lat) : reservation_station( Name,  Issue_Lat,  Execute_Lat){}
};

class beq_rs : public reservation_station {
};

class jalr_rs : public reservation_station {
};

class add_rs : public reservation_station {
};

class mull_rs : public reservation_station {
};
*/