#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <sstream>

template<typename T>
static bool compareTapeLengthDecrOrder(const Tape::Tape<T>* tape1, const Tape::Tape<T>* tape2) {
    return tape1->getSize() > tape2->getSize();
}

template<typename T>
void TapeSort::sort(Tape::Tape<T> *in, Tape::Tape<T> *out, const std::vector<Tape::Tape<T>*> &auxillary_tapes){

    in  -> rewind(0);

    const int max_vector_len = Config::MAX_RAM_BYTES / sizeof(T);

    if(in->getSize() <= max_vector_len){
        std::vector<T> values;
        values.push_back(in->read());
        for(size_t i=1; i<in->getSize(); i++){
            in->move(1);
            values.push_back(in->read());
        }
        std::sort(values.begin(), values.end());
        copy(values, out, Tape::FORWARDS);
        return;
    }

    if(auxillary_tapes.size() < 4){
        throw std::runtime_error("At least 4 auxillary tapes required.");
    }

    size_t aux_tape_min_size = in->getSize();

    std::vector<Tape::Tape<T>*> sorted_aux_tapes = auxillary_tapes;

    std::sort(sorted_aux_tapes.begin(), sorted_aux_tapes.end(), compareTapeLengthDecrOrder<T>);

    if(sorted_aux_tapes[3]->getSize() < aux_tape_min_size){
        throw std::runtime_error("At least 4 auxillary tapes of size " + to_string(aux_tape_min_size) + " required.");
    }


    //It is possible to use output tape instead of forwards[0] or backwards[0] and thus require less memory
    //But it is not stricly better approach

    //Two Merge-Forwards-Write-Backwards tapes
    Tape::Tape<T>* forwards[2] = {sorted_aux_tapes[0], sorted_aux_tapes[2]};
    std::stack<size_t>* forwards_chunks = new std::stack<size_t>[2];

    //Two Merge-Backwards-Write-Forwards tapes
    Tape::Tape<T>* backwards[2] = {sorted_aux_tapes[1], sorted_aux_tapes[3]};
    std::stack<size_t>* backwards_chunks = new std::stack<size_t>[2];

    backwards[0]->rewind(0);
    backwards[1]->rewind(0);

    std::vector<T> chunk;
    int cur_chunk_size = 1;
    chunk.push_back(in->read());
    
    int cur_tape = 0;
    for(size_t i=1; i<in->getSize(); i++){
        if(cur_chunk_size == max_vector_len || cur_chunk_size == backwards[cur_tape]->getSize()){
            std::sort(chunk.begin(), chunk.end());
            if(backwards_chunks[cur_tape].size()){
                backwards[cur_tape]->move(1);
            }
            copy(chunk, backwards[cur_tape], Tape::FORWARDS);
            backwards_chunks[cur_tape].push(cur_chunk_size);

            //alternate between 0 and 1
            cur_tape = 1 - cur_tape;
            cur_chunk_size = 0;
            chunk.clear();
        }
        in->move(1);
        chunk.push_back(in->read());
        cur_chunk_size++;
    }

    if(cur_chunk_size){
        std::sort(chunk.begin(), chunk.end());
        if(backwards_chunks[cur_tape].size()){
            backwards[cur_tape]->move(1);
        }
        copy(chunk, backwards[cur_tape], Tape::FORWARDS);
        backwards_chunks[cur_tape].push(cur_chunk_size);
    }

    forwards[0]->rewind(aux_tape_min_size-1);
    forwards[1]->rewind(aux_tape_min_size-1);


    auto choose_bigger = [](T vals[]) -> size_t {
        return vals[0] > vals[1] ? 0 : 1;
    };

    auto choose_leq = [](T vals[]) -> size_t { 
        return vals[0] <= vals[1] ? 0 : 1;
    };

    //chunks[0] >= chunks[1]
    while(true){
        if(backwards_chunks[0].size() == 1 && backwards_chunks[1].size() == 1){
            out->rewind(in->getSize()-1);
            merge_two_chunks<T>(backwards, backwards_chunks, out, Tape::BACKWARDS, choose_bigger);
            return;
        }
        merge<T>(backwards, backwards_chunks, forwards, forwards_chunks, Tape::BACKWARDS, choose_bigger);
        backwards[0]->rewind(0);
        backwards[1]->rewind(0);

        if(forwards_chunks[0].size() == 1 && forwards_chunks[1].size() == 1){
            out->rewind(0);
            merge_two_chunks<T>(forwards, forwards_chunks, out, Tape::FORWARDS, choose_leq);
            return;
        }
        merge<T>(forwards, forwards_chunks, backwards, backwards_chunks, Tape::FORWARDS, choose_leq);
        forwards[0]->rewind(aux_tape_min_size-1);
        forwards[1]->rewind(aux_tape_min_size-1);
    }

}

template<typename T>
static void merge_two_chunks(Tape::Tape<T>* in[], std::stack<size_t> *in_chunks,
                 Tape::Tape<T> *out, Tape::Direction direction, size_t (*choose)(T*))
{
    std::vector<size_t> num_vals = {in_chunks[0].top(), in_chunks[1].top()};

    T front_vals[2];
    bool val_read[2] = {false, false};
     
    while(num_vals[0] && num_vals[1]){
        for(int i=0; i<2; i++){
            if(!val_read[i]){
                front_vals[i] = in[i]->read();
                val_read[i] = true;
            }
        }
        size_t chosen_val = choose(front_vals);
        out->write( front_vals[chosen_val] );
        out->move(direction);
        val_read[chosen_val] = false;
        num_vals[chosen_val]--;

        if(num_vals[chosen_val]){
            in[chosen_val]->move(direction);
        }
    }
    for(int i=0; i<2; i++){
        copy(in[i], direction, out, direction, num_vals[i]);
    }
}

template<typename T>
static void merge(Tape::Tape<T>* in[], std::stack<size_t> *in_chunks,
           Tape::Tape<T>* out[], std::stack<size_t> *out_chunks,
           Tape::Direction direction, size_t (*choose)(T*)){

    // To which one of the two `out` tapes we will write to
    size_t write_to = 0;

    bool writable_position[2] = {true, true};

    //If there is an odd number of chunks to merge, the non-paired one must be in `in[0]`.
    //Therefore if there is one more chunk in the `in_chunks[1]` there is a paired chunk in `in_chunks[0]`.
    while(in_chunks[1].size()){

        size_t sorted_chunk_size = in_chunks[0].top() + in_chunks[1].top();
        
        if(!writable_position[write_to]){
            out[write_to]->move(direction);
        }

        merge_two_chunks(in, in_chunks, out[write_to], direction, choose);
        writable_position[write_to] = false;

        in_chunks[0].pop();
        in_chunks[1].pop();

        out_chunks[write_to].push(sorted_chunk_size);

        // `write_to` should alternate between 0 and 1
        write_to = 1 - write_to;

        if(!in_chunks[0].empty()){
            in[0]->move(direction);
        }
        if(!in_chunks[1].empty()){
            in[1]->move(direction);
        }
    }

    //If an odd number of chunks is given, a non-paired one must be on the first tape
    if(in_chunks[0].size()){
        out[write_to] -> move(direction);
        copy(in[0], direction, out[write_to], direction, in_chunks[0].top());
        out_chunks[write_to].push(in_chunks[0].top());
        in_chunks[0].pop();
    }
}