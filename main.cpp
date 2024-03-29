#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <stack>
#include <cmath>
using namespace std;

#include "tape.h"
#include "config.h"
#include "tape_sort.h"

int Config::MOVE_DELAY_MS = 0;
int Config::READ_DELAY_MS = 0;
int Config::WRITE_DELAY_MS = 0;
int Config::REWIND_DELAY_MS = 0;
int Config::MAX_RAM_BYTES = 0;

void create_file(string name, size_t size){
    ofstream file(name, std::ios::binary);

    if (file) {
        file.seekp(size - 1);
        file.put(0);
        file.close();
    } else {
        throw runtime_error("Cannot create file");
    }
}

int main(int argc, char** argv){

    try{
        Config::load_config("config.txt");
    } catch(exception &e){
        cerr << "Cannot load config: " << e.what();
        return 1;
    }

    if(Config::MOVE_DELAY_MS < 0 || Config::REWIND_DELAY_MS < 0 || Config::MAX_RAM_BYTES < 1 ||
       Config::READ_DELAY_MS < 0 || Config::WRITE_DELAY_MS < 0){
        cerr << "Incorrect config values, aborting" << endl;
        return 1;
    }
    
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " input_tape output_tape" << endl;
        return 1;
    }

    Tape::FileTape<int32_t> *input, *output;

    try{
        input = new Tape::FileTape<int32_t>(argv[1]);
    } catch(const exception *e){
        cerr << "Cannot open input file: " << e->what();
        return 1;
    }

    try{
        create_file(argv[2], input->getSize() * sizeof(int32_t));
        output = new Tape::FileTape<int32_t>(argv[2]);
    } catch(const exception *e){
        input -> close();   
        cerr << "Cannot create or open output file: " << e->what();
        return 1;
    }
    
    


    vector<Tape::Tape<int32_t>*> aux_tapes;

    //We need 4 auxillary tapes
    try{
        for(int i=0; i<4; i++){
            string filename = "temp/" + to_string(i) + ".tape";
            create_file(filename, input->getSize() * sizeof(int32_t));
            aux_tapes.push_back(new Tape::FileTape<int32_t>(filename));
        }
    } catch(exception &e){
        input->close();
        output->close();
        for(size_t i=0; i<aux_tapes.size(); i++) 
            ((Tape::FileTape<int32_t>*) aux_tapes[i])->close();

        cerr << "Cannot create auxillary files: " << e.what() << endl;
        return 1;
    }

    

    TapeSort::sort(input, output, aux_tapes);


    input->close();
    output->close();
    for(size_t i=0; i<aux_tapes.size(); i++) 
        ((Tape::FileTape<int32_t>*) aux_tapes[i])->close();

    

   /*
    vector<int> in_v;
    for(int i=1; i<=100; i++) in_v.push_back(101-i);
    Tape::ArrayTape<int32_t> *in = new Tape::ArrayTape<int32_t>(in_v);
    Tape::ArrayTape<int32_t> *out = new Tape::ArrayTape<int32_t>(in->getSize());
    vector<Tape::Tape<int32_t>*> aux;
    size_t long_tape_size = 99;
    size_t short_tape_size = 99;
    
    aux.push_back(new Tape::ArrayTape<int32_t>(long_tape_size));
    aux.push_back(new Tape::ArrayTape<int32_t>(long_tape_size));
    aux.push_back(new Tape::ArrayTape<int32_t>(short_tape_size));
    aux.push_back(new Tape::ArrayTape<int32_t>(short_tape_size));
    TapeSort::sort(in, out, aux, 3);
    out->print();
    */
    
    return 0;
}