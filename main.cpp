#include "includes.h"
#include "instruction.cpp"
#include "Register.cpp"
#include "reservation_station.cpp"
#include "CDB.cpp"

int instr_issue = 0;
vector<reservation_station> writebacks;
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

CDB result;

int Mem[10] = {1,2,3,4} ; //Data Memory

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
    vector<vector<reservation_station>> Reservation_Stations;
    vector<reservation_station> LW_units;
    vector<reservation_station> SW_units;
    vector<reservation_station> BEQ_units;
    vector<reservation_station> JALR_units;
    vector<reservation_station> ADD_units;
    vector<reservation_station> MULL_units;

    Reservation_Stations.push_back(LW_units);   // 0
    Reservation_Stations.push_back(SW_units);   // 1
    Reservation_Stations.push_back(BEQ_units);  // 2
    Reservation_Stations.push_back(JALR_units); // 3
    Reservation_Stations.push_back(ADD_units);  // 4
    Reservation_Stations.push_back(MULL_units); // 5

    for (int i = 0; i < Num_LW_RS; i++){
        reservation_station LW("LW" + to_string(i + 1), 3,1);
        Reservation_Stations[0].push_back(LW);
    }
    for (int i = 0; i < Num_SW_RS; i++){
        reservation_station SW("SW" + to_string(i+1), 3,1);
        Reservation_Stations[1].push_back(SW);
    }
    for (int i = 0; i < Num_BEQ_RS; i++){
        reservation_station BEQ("BEQ" + to_string(i+1), 1,1);
        Reservation_Stations[2].push_back(BEQ);
    }
    for (int i = 0; i < Num_JALR_RS; i++){
        reservation_station JALR("JALR" + to_string(i+1), 1,1);
        Reservation_Stations[3].push_back(JALR);
    }
    for (int i = 0; i < Num_ADD_RS; i++){
        reservation_station ADD("ADD" + to_string(i+1), 1,2);
        Reservation_Stations[4].push_back(ADD);
    }
    for (int i = 0; i < Num_MULL_RS; i++){
        reservation_station MULL("MULL" + to_string(i+1), 1,6);
        Reservation_Stations[5].push_back(MULL);
    }

    //build register status table
    vector<Register> Registers_Status;
    for (int i = 0; i < Num_REG; i++){
        Register Reg;
        Registers_Status.push_back(Reg);
    }

    // initialize register file
    vector<int> Regs = {0,1,2,3,4,5,6,7};

    queue<string> LoadStoreQueue;

}

/*
while( num_writbacks < Instructions.size() ) 
clk++;
ISSUE
Execute
Write

*/

void ISSUE(vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue){
    string Op = Instructions[instr_issue].type;
    bool free_flag = false;
    int unit_num;
    
    switch (Op) {// FP OPERATION (R format, ass,sub,mul)
    ///check the there is a station empty get its index r
        case ("ADD"): //4
            for (int i = 0; i < Reservation_Stations[4].size(); i++) {
                if (Reservation_Stations[4][i].busy == false) {
                    if (Registers_Status[Instructions[instr_issue].rs1].Qi != 0) //waiting for reservation station (rs1 not ready)
                        Reservation_Stations[4][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                    else {
                        Reservation_Stations[4][i].Vj = Regs[Instructions[instr_issue].rs1];
                        Reservation_Stations[4][i].Qj = 0;
                    }

                    if (Registers_Status[Instructions[instr_issue].rs2].Qi != 0) //waiting for reservation station (rs1 not ready)
                        Reservation_Stations[4][i].Qk = Registers_Status[Instructions[instr_issue].rs2].Qi;
                    else {
                        Reservation_Stations[4][i].Vk = Regs[Instructions[instr_issue].rs2];
                        Reservation_Stations[4][i].Qk = 0;
                    }

                    Reservation_Stations[4][i].busy = true;
                    Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[4][i].name;
                    
                    Reservation_Stations[4][i].instr_num = instr_issue;
                    Instructions[instr_issue].issued = clk;
                    Reservation_Stations[4][i].issue_lat--;

                    Instructions[instr_issue].status = "i";
                    instr_issue++;
                    break;
                }
            }
            break;

    

    // LOAD

    if(Registers_Status[Instructions[i].rs1].Qi != 0 ) //waiting for reservation station (rs1 not ready)
        Reservation_Stations[0][r].Qj = Registers_Status[Instructions[i].rs1].Qi;
    else {
        Reservation_Stations[0][r].Vj = Regs[Instructions[i].rs1];
        Reservation_Stations[0][r].Qj = 0;
    }

    Reservation_Stations[0][r].A = Instructions[i].imm;
    Reservation_Stations[0][r].busy = true;
    Registers_Status[Instructions[i].rd].Qi = Reservation_Stations[0][r].name;
    //load store queue
    Reservation_Stations[0][r].inst = Instructions[i].inst;
    LoadStoreQueue.push(Instructions[i].inst);

    // STORE
    if(Registers_Status[Instructions[i].rs1].Qi != 0 ) //waiting for reservation station (rs1 not ready)
        Reservation_Stations[0][r].Qj = Registers_Status[Instructions[i].rs1].Qi;
    else {
        Reservation_Stations[0][r].Vj = Regs[Instructions[i].rs1];
        Reservation_Stations[0][r].Qj = 0;
    }

    if(Registers_Status[Instructions[i].rs2].Qi != 0 ) //waiting for reservation station (rs1 not ready)
        Reservation_Stations[0][r].Qk = Registers_Status[Instructions[i].rs2].Qi;
    else {
        Reservation_Stations[0][r].Vk = Regs[Instructions[i].rs2];
        Reservation_Stations[0][r].Qk = 0;
    }

    Reservation_Stations[0][r].A = Instructions[i].imm;
    Reservation_Stations[0][r].busy = true;

    //load store queue
    Reservation_Stations[0][r].inst = Instructions[i].inst;
    LoadStoreQueue.push(Instructions[i].inst);

}

void EXECUTE (vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue) {
    
    for (int i = 0; i < Reservation_Stations.size(); i++)
        for (int j = 0; j < Reservation_Stations[i].size; j++)
        {
            if (Reservation_Stations[i][j].busy)
            {
                if (Instructions[Reservation_Stations[i][j].instr_num].status == "e") {
                    // if FP
                    if (Instructions[Reservation_Stations[i][j].instr_num].type == "ADD" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "MULL" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "MULH" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "BEQ")
                            if (Reservation_Stations[0][r].Qj == 0 && Reservation_Stations[0][r].Qk == 0) {
                                if (Reservation_Stations[i][j].execute_lat > 0) {

                                    if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                    Reservation_Stations[i][j].execute_lat--;
                                }
                                if (Reservation_Stations[i][j].execute_lat == 0) {
                                    Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                }
                                 
                            }
                    else if (Instructions[Reservation_Stations[i][j].instr_num].type == "ADDI" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "NEG" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "JALR") {
                                if (Reservation_Stations[0][r].Qj == 0) {
                                    if (Reservation_Stations[i][j].execute_lat > 0) {

                                        if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                            Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                        Reservation_Stations[i][j].execute_lat--;
                                    }
                                    if (Reservation_Stations[i][j].execute_lat == 0) {
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                    }
                                }}
                    else if (Instructions[Reservation_Stations[i][j].instr_num].type == "LW" || 
                            Instructions[Reservation_Stations[i][j].instr_num].type == "SW" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "RET") {
                                if (Reservation_Stations[i][j].execute_lat > 0) {

                                    if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                    Reservation_Stations[i][j].execute_lat--;
                                }
                                if (Reservation_Stations[i][j].execute_lat == 0) {
                                    Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                }
                            }                                
                }
                if (Reservation_Stations[i][j].issue_lat > 0)
                    if (Reservation_Stations[0][r].Qj == 0 && LoadStoreQueue.front() == Reservation_Stations[0][r].inst)
                        Reservation_Stations[i][j].issue_lat--;
                if (Reservation_Stations[i][j].issue_lat == 0 && Instructions[Reservation_Stations[i][j].instr_num].status == "i")
                    Instructions[Reservation_Stations[i][j].instr_num].status = "e";

                
            } 
          
        }
   // ( "ADD" ||  == "NEG" ||  == "ADDI" ||  == "MULL" ||  == "MULH")
    // FP OPERATION 


    // status i       issue_lat = 0
    
    // status i       issue_lat -> 2 -> 1 -> 0





   /* // LOAD
    if (Reservation_Stations[0][r].Qj == 0 && LoadStoreQueue.front() == Reservation_Stations[0][r].inst)
        Reservation_Stations[0][r].A = Reservation_Stations[0][r].Vj + Reservation_Stations[0][r].A;
    
    //Mem[Reservation_Stations[0][r].A]

    // STORE
    if (Reservation_Stations[0][r].Qj == 0 && LoadStoreQueue.front() == Reservation_Stations[0][r].inst)
        Reservation_Stations[0][r].A = Reservation_Stations[0][r].Vj + Reservation_Stations[0][r].A;
    */
    
}

void WRITE (vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue) {
    // FP OPERATION, LOAD
    for (int i = 0; i < Reservation_Stations.size(); i++)
        for (int j = 0; j < Reservation_Stations[i].size; j++)
        {
            if (Reservation_Stations[i][j].busy) {
                if (Instructions[Reservation_Stations[i][j].instr_num].status == "w") {
                    //add to vector
                    writebacks.push_back(Reservation_Stations[i][j]);
                }

                if (Reservation_Stations[i][j].execute_lat == 0 && Instructions[Reservation_Stations[i][j].instr_num].status == "e")
                    Instructions[Reservation_Stations[i][j].instr_num].status = "w";
            }
        }
    //sort
    sort(writebacks.begin(), writebacks.size());
    int inst = writebacks[0].instr_num;
    string Op = Instructions[inst].type;
    switch(Op) {
        case "ADD":
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].name) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].name) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].name) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                }
            }
 //--------------------------------------------------------------------------------------------

    // execute first vectors filled 
    for(int i = 1; i < Num_REG; i++){
        if(Registers_Status[i].Qi == Reservation_Stations[0][r].name) {
            
            Registers_Status[i].Qi = 0;
        }
    }

    for(int i = 0; i < Reservation_Stations.size(); i++){
        for (int j = 0; j < Reservation_Stations[i].size(); j++){
            if(Reservation_Stations[i][j].Qj == Reservation_Stations[0][r].name) {
                Reservation_Stations[0][r].Vj = result.value;
                Reservation_Stations[0][r].Qj = 0;
            }
        }
    }

    for(int i = 0; i < Reservation_Stations.size(); i++){
        for (int j = 0; j < Reservation_Stations[i].size(); j++){
            if(Reservation_Stations[i][j].Qk == Reservation_Stations[0][r].name) {
                Reservation_Stations[0][r].Vk = result.value;
                Reservation_Stations[0][r].Qk = 0;
            }
        }
    }
    
    Reservation_Stations[0][r].busy = false;

    // STORE
    if(Reservation_Stations[0][r].Qk == 0){
        Mem[Reservation_Stations[0][r].A] = Reservation_Stations[0][r].Vk;
        Reservation_Stations[0][r].busy = false;
    }
}