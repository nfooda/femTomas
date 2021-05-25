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
    }
};