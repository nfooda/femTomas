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

int branch_num = 0;
int mispredection = 0;
int PC;

bool BEQ_flag = false;
bool JALR_flag = false;
bool RET_flag = false;

int before_jump_address;

//main functions
void issue(vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue);
void execute(vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue);
void write(vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue);

void print(vector<instruction>& Instructions);

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

    int count_units=1 ;
    for (int i = 0; i < Num_LW_RS; i++){
        reservation_station LW("LW" + to_string(i + 1), 3,1, count_units);
        Reservation_Stations[0].push_back(LW);
        count_units++;
    }
    for (int i = 0; i < Num_SW_RS; i++){
        reservation_station SW("SW" + to_string(i+1), 3,1, count_units);
        Reservation_Stations[1].push_back(SW);
        count_units++;
    }
    for (int i = 0; i < Num_BEQ_RS; i++){
        reservation_station BEQ("BEQ" + to_string(i+1), 1,1, count_units);
        Reservation_Stations[2].push_back(BEQ);
        count_units++;
    }
    for (int i = 0; i < Num_JALR_RS; i++){
        reservation_station JALR("JALR" + to_string(i+1), 1,1, count_units);
        Reservation_Stations[3].push_back(JALR);
        count_units++;
    }
    for (int i = 0; i < Num_ADD_RS; i++){
        reservation_station ADD("ADD" + to_string(i+1), 1,2, count_units);
        Reservation_Stations[4].push_back(ADD);
        count_units++;
    }
    for (int i = 0; i < Num_MULL_RS; i++){
        reservation_station MULL("MULL" + to_string(i+1), 1,6, count_units);
        Reservation_Stations[5].push_back(MULL);
        count_units++;
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


    do{
        clk++; 

        ISSUE(Instructions,Reservation_Stations,Registers_Status,Regs, LoadStoreQueue);
		EXECUTE(Instructions,Reservation_Stations,Registers_Status,Regs, LoadStoreQueue);
		WRITE(Instructions,Reservation_Stations,Registers_Status,Regs, LoadStoreQueue);

       
        done = false;
        if(num_writbacks == Instructions.size())
            done = true;
        cout << endl;
	}while(!done);

    print (Instructions);


    return 0;

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
    int unit_num;

    if(BEQ_flag || JALR_flag || RET_flag)
        instr_issue = PC;

    
   // FP OPERATION (R format, ass,sub,mul)
    ///check the there is a station empty get its index r
    if(Op == "ADD"){ //4
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
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[4][i].num;
                    
                Reservation_Stations[4][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    if(Op == "ADDI"){ //4
        for (int i = 0; i < Reservation_Stations[4].size(); i++) {
            if (Reservation_Stations[4][i].busy == false) {
                if (Registers_Status[Instructions[instr_issue].rs1].Qi != 0) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[4][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[4][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[4][i].Qj = 0;
                }

                Reservation_Stations[4][i].busy = true;
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[4][i].num;
                    
                Reservation_Stations[4][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    if(Op == "NEG"){ //4
        for (int i = 0; i < Reservation_Stations[4].size(); i++) {
            if (Reservation_Stations[4][i].busy == false) {
                if (Registers_Status[Instructions[instr_issue].rs1].Qi != 0) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[4][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[4][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[4][i].Qj = 0;
                }

                Reservation_Stations[4][i].busy = true;
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[4][i].num;
                    
                Reservation_Stations[4][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    if(Op == "MULL" || Op == "MULH"){ //5
        for (int i = 0; i < Reservation_Stations[5].size(); i++) {
            if (Reservation_Stations[5][i].busy == false) {
                if (Registers_Status[Instructions[instr_issue].rs1].Qi != 0) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[5][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[5][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[5][i].Qj = 0;
                }

                Reservation_Stations[5][i].busy = true;
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[5][i].num;
                    
                Reservation_Stations[5][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    // LOAD
    if(Op == "LW"){ //0
        for (int i = 0; i < Reservation_Stations[0].size(); i++) {
            if (Reservation_Stations[0][i].busy == false) {

                if(Registers_Status[Instructions[instr_issue].rs1].Qi != 0 ) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[0][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[0][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[0][i].Qj = 0;
                }

                Reservation_Stations[0][i].A = Instructions[instr_issue].imm;
                Reservation_Stations[0][i].busy = true;
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[0][i].num;
                //load store queue
                Reservation_Stations[0][i].inst = Instructions[instr_issue].inst;
                LoadStoreQueue.push(Instructions[instr_issue].inst);

                Reservation_Stations[0][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }
    // STORE
    if(Op == "SW") { //1
        for (int i = 0; i < Reservation_Stations[1].size(); i++) {
            if (Reservation_Stations[1][i].busy == false) {

                if(Registers_Status[Instructions[instr_issue].rs1].Qi != 0 ) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[1][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[1][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[1][i].Qj = 0;
                }

                if(Registers_Status[Instructions[instr_issue].rs2].Qi != 0 ) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[1][i].Qk = Registers_Status[Instructions[instr_issue].rs2].Qi;
                else {
                    Reservation_Stations[1][i].Vk = Regs[Instructions[instr_issue].rs2];
                    Reservation_Stations[1][i].Qk = 0;
                }

                Reservation_Stations[1][i].A = Instructions[instr_issue].imm;
                Reservation_Stations[1][i].busy = true;
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[1][i].num;

                //load store queue
                Reservation_Stations[1][i].inst = Instructions[instr_issue].inst;
                LoadStoreQueue.push(Instructions[instr_issue].inst);

                Reservation_Stations[1][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    //BEQ
    if(Op == "BEQ"){ //2
        for (int i = 0; i < Reservation_Stations[2].size(); i++) {
            if (Reservation_Stations[2][i].busy == false) {

                if(Registers_Status[Instructions[instr_issue].rs1].Qi != 0 ) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[2][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[2][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[2][i].Qj = 0;
                }

                if(Registers_Status[Instructions[instr_issue].rs2].Qi != 0 ) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[2][i].Qk = Registers_Status[Instructions[instr_issue].rs2].Qi;
                else {
                    Reservation_Stations[2][i].Vk = Regs[Instructions[instr_issue].rs2];
                    Reservation_Stations[2][i].Qk = 0;
                }

                Reservation_Stations[2][i].A = Instructions[instr_issue].imm;
                Reservation_Stations[2][i].busy = true;
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[2][i].num;

                Reservation_Stations[2][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    if(Op == "JALR"){ //3
        for (int i = 0; i < Reservation_Stations[3].size(); i++) {
            if (Reservation_Stations[3][i].busy == false) {
                if (Registers_Status[Instructions[instr_issue].rs1].Qi != 0) //waiting for reservation station (rs1 not ready)
                    Reservation_Stations[3][i].Qj = Registers_Status[Instructions[instr_issue].rs1].Qi;
                else {
                    Reservation_Stations[3][i].Vj = Regs[Instructions[instr_issue].rs1];
                    Reservation_Stations[3][i].Qj = 0;
                }

                Reservation_Stations[3][i].busy = true;
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[3][i].num;
                    
                Reservation_Stations[3][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

    if(Op == "RET"){ //3
        for (int i = 0; i < Reservation_Stations[4].size(); i++) {
            if (Reservation_Stations[3][i].busy == false) {
               
                Reservation_Stations[3][i].busy = true;
        ///////////////////////////////////////////////////////
                Registers_Status[Instructions[instr_issue].rd].Qi = Reservation_Stations[3][i].num;
                    
                Reservation_Stations[3][i].instr_num = instr_issue;
                Instructions[instr_issue].issued = clk;
                Instructions[instr_issue].issue_lat--;

                Instructions[instr_issue].status = "i";
                instr_issue++;
                break;
            }
        }
    }

}

///////////////////////////////
void EXECUTE (vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue) {
    
    if(BEQ_flag || JALR_flag || RET_flag){
        for (int i = 0; i < Reservation_Stations.size(); i++)
            for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                if( Reservation_Stations[i][j].instr_num == before_jump_address) {
                    Reservation_Stations[i][j].notBusy();
                }
            }
    }
    
    
    for (int i = 0; i < Reservation_Stations.size(); i++)
        for (int j = 0; j < Reservation_Stations[i].size(); j++)
        {
            if (Reservation_Stations[i][j].busy)
            {
                if (Instructions[Reservation_Stations[i][j].instr_num].status == "e") {
                    // if FP
                    if (Instructions[Reservation_Stations[i][j].instr_num].type == "ADD" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "MULL" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "MULH" ||
                        Instructions[Reservation_Stations[i][j].instr_num].type == "BEQ")
                            if (Reservation_Stations[i][j].Qj == 0 && Reservation_Stations[i][j].Qk == 0) {
                                if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat > 0) {

                                    if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                    Instructions[Reservation_Stations[i][j].instr_num].execute_lat--;
                                }
                                if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat == 0) {
                                    Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                }
                                 
                            }
                    else if (Instructions[Reservation_Stations[i][j].instr_num].type == "ADDI" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "NEG" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "JALR") {
                                if (Reservation_Stations[i][j].Qj == 0) {
                                    if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat > 0) {

                                        if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                            Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                        Instructions[Reservation_Stations[i][j].instr_num].execute_lat--;
                                    }
                                    if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat == 0) {
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                    }
                                }}
                    else if (Instructions[Reservation_Stations[i][j].instr_num].type == "LW" || 
                            Instructions[Reservation_Stations[i][j].instr_num].type == "SW" ||
                            Instructions[Reservation_Stations[i][j].instr_num].type == "RET") {
                                if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat > 0) {

                                    if (Instructions[Reservation_Stations[i][j].instr_num].execution_started == 0)
                                        Instructions[Reservation_Stations[i][j].instr_num].execution_started = clk;

                                    Instructions[Reservation_Stations[i][j].instr_num].execute_lat--;
                                }
                                if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat == 0) {
                                    Instructions[Reservation_Stations[i][j].instr_num].execution_finished = clk;
                                }
                            }                                
                }
                if (Instructions[Reservation_Stations[i][j].instr_num].issue_lat > 0)
                    if (Reservation_Stations[i][j].Qj == 0 && LoadStoreQueue.front() == Reservation_Stations[i][j].inst)
                        Instructions[Reservation_Stations[i][j].instr_num].issue_lat--;
                if (Instructions[Reservation_Stations[i][j].instr_num].issue_lat == 0 && Instructions[Reservation_Stations[i][j].instr_num].status == "i")
                    Instructions[Reservation_Stations[i][j].instr_num].status = "e";
                
            } 
          
        }
}

void WRITE (vector<instruction>& Instructions, vector<vector<reservation_station>>& Reservation_Stations, vector<Register>& Registers_Status, vector<int>& Regs, queue<string>& LoadStoreQueue) {
    
    if(BEQ_flag || JALR_flag || RET_flag){
        for (int i = 0; i < Reservation_Stations.size(); i++)
            for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                if( Reservation_Stations[i][j].instr_num == before_jump_address) {
                    Reservation_Stations[i][j].notBusy();
                }
            }
        BEQ_flag = false;
        JALR_flag = false;
        RET_flag = false;
    }
    
    // FP OPERATION, LOAD
    for (int i = 0; i < Reservation_Stations.size(); i++)
        for (int j = 0; j < Reservation_Stations[i].size(); j++)
        {
            if (Reservation_Stations[i][j].busy) {
                if (Instructions[Reservation_Stations[i][j].instr_num].status == "w") {
                    //add to vector
                    writebacks.push_back(Reservation_Stations[i][j]);
                }

                if (Instructions[Reservation_Stations[i][j].instr_num].execute_lat == 0 && Instructions[Reservation_Stations[i][j].instr_num].status == "e")
                    Instructions[Reservation_Stations[i][j].instr_num].status = "w";
            }
        }
    //sort
    if(writebacks.size() > 0) {
        sort(writebacks.begin(), writebacks.end());
        int inst = writebacks[0].instr_num;
        string Op = Instructions[inst].type;

        if(Op == "ADD") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            Regs[Instructions[inst].rd] = Regs[Instructions[inst].rs1] + Regs[Instructions[inst].rs2];
        }

        if(Op == "NEG") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            Regs[Instructions[inst].rd] = -Regs[Instructions[inst].rs1];
        }

        if(Op == "ADDI") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            Regs[Instructions[inst].rd] = Regs[Instructions[inst].rs1] + Instructions[inst].imm;
        }

        if(Op == "MULL") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            //Regs[Instructions[inst].rd] = mull(Regs[Instructions[inst].rs1], Regs[Instructions[inst].rs2]);
            Regs[Instructions[inst].rd] = Regs[Instructions[inst].rs1]* Regs[Instructions[inst].rs2];
        }

        if(Op == "MULH") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            //Regs[Instructions[inst].rd] = mulh(Regs[Instructions[inst].rs1], Regs[Instructions[inst].rs2]);
            Regs[Instructions[inst].rd] = Regs[Instructions[inst].rs1]* Regs[Instructions[inst].rs2];
        }

        if(Op == "LW") {
            for (int i = 1; i < Num_REG; i++) {
                if (Registers_Status[i].Qi == writebacks[0].num) {
                    Registers_Status[i].Qi = 0;
                }
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if (Reservation_Stations[i][j].Qj == writebacks[0].num) {
                        Reservation_Stations[i][j].Qj = 0;
                    }
                    if (Reservation_Stations[i][j].Qk == writebacks[0].num) {
                        Reservation_Stations[i][j].Qk = 0;
                    }
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            Regs[Instructions[inst].rd] = Mem[Regs[Instructions[inst].rs1]+ Instructions[inst].imm];
        }

        if(Op == "SW") {
            if(writebacks[0].Qk == 0){
                Mem[Regs[Instructions[inst].rs1] + Instructions[inst].imm] = Instructions[inst].rs2;
            }
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
        }

        if(Op == "BEQ") {
            branch_num++;
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            if(Regs[Instructions[inst].rs1] == Regs[Instructions[inst].rs2]){
                PC = Instructions[inst].imm;
                mispredection ++;

                BEQ_flag = true;
                before_jump_address = inst+1;

            }
        }

        if(Op == "JALR") {
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            Regs[1] = inst+1;
            PC = Regs[Instructions[inst].rs1];
            
            before_jump_address = inst+1;

            JALR_flag = true;
        }

        if(Op == "RET") {
            for (int i = 0; i < Reservation_Stations.size(); i++) {
                for (int j = 0; j < Reservation_Stations[i].size(); j++) {
                    if(Reservation_Stations[i][j].name == writebacks[0].name)
                        Reservation_Stations[i][j].notBusy();
                }
            }
            PC = Regs[1];
            RET_flag = true;

            before_jump_address = inst+1;

        }
        Instructions[inst].written = clk;
        num_writbacks++;
        writebacks.erase(writebacks.begin());
    }
}


void print (vector<instruction>& Instructions) {
    cout << setw(25)<<"Instruction" << setw(15)<<"issued"<<setw(15)<<"execution start" << setw(17)<<"execution finish"<<setw(15)<<"writeback"<<endl;
    for(int i = 0; i < Instructions.size(); i++){
        cout << setw(25)<<Instructions[i].inst << setw(15)<<Instructions[i].issued<<setw(15)<< Instructions[i].execution_started << setw(17)<<Instructions[i].execution_finished<<setw(15)<<Instructions[i].written<<endl;
    }

    cout <<endl;
    cout << "Total execution time: "<<clk<<" clock cycles"<<endl;
    cout << "IPC: "<<Instructions.size()/clk<<endl;
    cout << "The branch misprediction percentage: "<<mispredection/branch_num*100<<" %"<<endl;
}
