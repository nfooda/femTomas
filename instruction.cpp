#include "includes.h"

class instruction {
    public:
        string inst;
	    int rs1, rs2, rd, imm;
	    int issued, execution_started, execution_finished, written;
        string type;
	    string status;
        int issue_lat;
        int execute_lat;

    instruction() {
        rs1 = -1;
        rs2 = -1;
        rd = -1;
        imm = -1;
        issued = 0;
        execution_started = 0;
        execution_finished = 0;
        written = 0;
        inst = "";
        type = "";
        status = "";
        issue_lat = -1;
        execute_lat = -1;
    }

    instruction(string Inst, string Type, int RS1,int RS2, int RD, int IMM) {
        rs1 = RS1;
        rs2 = RS2;
        rd = RD;
        imm = IMM;
        issued = 0;
        execution_started = 0;
        execution_finished = 0;
        written = 0;
        inst = Inst;
        type = Type;
        status = "";
        issue_lat = -1;
        execute_lat = -1;
    }

    void parser(string Inst){
        inst = Inst;
        // build a vector of all parts of the instruction
        vector<string> parts;
        string part;
        for (int i = 0; i<Inst.size(); i++){
            if (Inst[i] == ',' || Inst[i] == ')')
                Inst.erase(Inst.begin() + i);
            if (Inst[i] == '(')
                Inst.replace(i, 1, " ");
            if (inst[i] == 'R' && inst[i+1] != ' ')
                Inst.erase(Inst.begin() + i);

        }

        stringstream str (Inst);
        while (str >> part)
            parts.push_back(part);

        type = parts[0];

        if (parts[0] == "ADD" || parts[0] == "NEG" || parts[0] == "ADDI") {
            rd = stoi (parts[1]);
            rs1 = stoi (parts[2]);
            issue_lat = 1;
            execute_lat = 2;
            if(parts[0] == "ADD")
                rs2 = stoi (parts[3]);
            if (parts[0] == "ADDI")
                imm = stoi (parts[3]);
        }

        if (parts[0] == "MULL" || parts[0] == "MULH") {
            rd = stoi (parts[1]);
            rs1 = stoi (parts[2]);
            rs2 = stoi (parts[3]);
            issue_lat = 1;
            execute_lat = 6;
        }


        if (parts[0] == "LW" || parts[0] == "SW") {
            imm = stoi (parts[2]);
            rs1 = stoi (parts[3]);
            issue_lat = 3;
            execute_lat = 1;
            if (parts[0] == "LW")
                rd = stoi (parts[1]);
            if (parts[0] == "SW")
                rs2 = stoi (parts[1]);
        }

        if (parts[0] == "BEQ"){
            rs1 = stoi (parts[1]);
            rs2 = stoi (parts[2]);
            imm = stoi (parts[3]);
            issue_lat = 1;
            execute_lat = 1;
        }

        if(parts[0] == "JALR") {
            rs1 = stoi (parts[1]);
            issue_lat = 1;
            execute_lat = 1;
        }

        if(parts[0] == "RET") {
            issue_lat = 1;
            execute_lat = 1;
        }

    }
};