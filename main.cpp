#include "includes.h"
#include "instruction.cpp"
#include "Register.cpp"
#include "reservation_station.cpp"

//number of reservation stations
const int Num_LW_RS = 2;
const int Num_SW_RS = 2;
const int Num_BEQ_RS = 1;
const int Num_JALR_RS = 1;
const int Num_ADD_RS = 2;
const int Num_MULL_RS = 1;
//number of registers
const int Num_REG = 8;

//important
int clk = 0; //global clock counter
int num_writbacks = 0;
int num_inst = 0; 
bool done = false; //to finish the program when num_inst = num_writbacks

//main functions
void issue();
void execute();
void write();

void print();

main () {

    //build instruction table vector
    vector<instruction> Instructions;
    string file_name;
    cout << "Enter instructions file name. \n";
    cin >> file_name;

    ifstream file(file_name);
    string inst;
    while (getline(file, inst)) {
       instruction INST;
       INST.parser(inst);
       Instructions.push_back(INST);
    }

    //build reservation stations table
    //reservation_station(string Name, int Issue_Lat, int Execute_Lat)
    vector<reservation_station> Reservation_Stations;
    for (int i = 0; i < Num_LW_RS; i++){
        reservation_station LW("LW", 2,1);
        Reservation_Stations.push_back(LW);
    }
    for (int i = 0; i < Num_SW_RS; i++){
        reservation_station SW("SW" + to_string(i+1), 2,1);
        Reservation_Stations.push_back(SW);
    }
    for (int i = 0; i < Num_BEQ_RS; i++){
        reservation_station BEQ("BEQ" + to_string(i+1), 1,1);
        Reservation_Stations.push_back(BEQ);
    }
    for (int i = 0; i < Num_JALR_RS; i++){
        reservation_station JALR("JALR" + to_string(i+1), 1,1);
        Reservation_Stations.push_back(JALR);
    }
    for (int i = 0; i < Num_ADD_RS; i++){
        reservation_station ADD("ADD" + to_string(i+1), 1,2);
        Reservation_Stations.push_back(ADD);
    }
    for (int i = 0; i < Num_MULL_RS; i++){
        reservation_station MULL("MULL" + to_string(i+1), 1,6);
        Reservation_Stations.push_back(MULL);
    }

    //build register status table
    vector<Register> Registers_Status;
    for (int i = 0; i < Num_REG; i++){
        Register Reg;
        Registers_Status.push_back(Reg);
    }

}
