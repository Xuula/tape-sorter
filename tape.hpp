template<typename T>
void copy(Tape<T> *src, Direction read_dir, Tape<T> *dest, Direction write_dir, size_t num_vals){
    if(num_vals > 0){
        dest->write(src->read());
        for(size_t i=1; i<num_vals; i++){
            src->move(read_dir);
            dest->move(write_dir);
            dest->write(src->read());
        }
    }
}

template<typename T>
void copy(const std::vector<T> &values, Tape<T> *dest, Direction write_direction){
    if(values.size() > 0){
        dest->write(values[0]);
        for(size_t i=1; i<values.size(); i++){
            dest->move(write_direction);
            dest->write(values[i]);
        }
    }
}

template <typename T>
FileTape<T>::FileTape(const std::string& filename)
: size(file_capacity(filename))
{
    try{
        file.open(filename, std::ios::in|std::ios::out|std::ios::binary);
    } catch (std::exception &e){
        throw std::runtime_error("Failed to open file: " + filename + ".");
    }

    if(!file.is_open()){
        throw std::runtime_error("Failed to open file: " + filename + ".");
    }
    position = 0;
}

template<typename T>
void FileTape<T>::close(){
    file.close();
}

template<typename T>
size_t FileTape<T>::getSize() const{
    return size;
}

template<typename T>
size_t FileTape<T>::getPosition() const{
    return position;
}

template<typename T>
void FileTape<T>::move(const int& steps){
    int new_pos = position + steps;
    if(new_pos < 0 || new_pos >= size){
        throw std::runtime_error("Failed to move tape: trying to move beyond the tape's borders.");
    }
    position = new_pos;

    
    file.seekg(steps*sizeof(T), std::ios_base::cur);

    usleep(Config::MOVE_DELAY_MS);
}

template<typename T>
void FileTape<T>::rewind(const size_t& new_pos){
    if(new_pos < 0 || new_pos >= size){
        throw std::runtime_error("Failed to rewind the tape: invalid position");
    }

    
    position = new_pos;
    file.seekg(new_pos*sizeof(T), std::ios_base::beg);
    file.seekp(new_pos*sizeof(T), std::ios_base::beg);
    
   /*
    position = 0;
    file.seekg(0);
    file.seekp(0);
    move(new_pos);
    */

    usleep(Config::REWIND_DELAY_MS);
}

template<typename T>
T FileTape<T>::read(){
    T data;
    file.read(reinterpret_cast<char*>(&data), sizeof(T));
    file.seekg(-sizeof(T), std::ios_base::cur);

    usleep(Config::READ_DELAY_MS * 1000);

    return data;
}

template<typename T>
void FileTape<T>::write(const T& data){
    file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    file.seekp(-sizeof(T), std::ios_base::cur);

    usleep(Config::WRITE_DELAY_MS * 1000);
}

template<typename T>
size_t FileTape<T>::file_capacity(const std::string& filename){
    return std::filesystem::file_size(filename) / sizeof(T);
}






template<typename T>
ArrayTape<T>::ArrayTape(size_t size){
    array = std::vector<T>(size);
    position = 0;
}

template<typename T>
ArrayTape<T>::ArrayTape(const std::vector<T> &array){
    this->array = array;
    position = 0;
}

template<typename T>
size_t ArrayTape<T>::getSize() const{
    return array.size();
}

template<typename T>
size_t ArrayTape<T>::getPosition() const {
    return position;
}

template<typename T>
void ArrayTape<T>::move(const int& steps){
    int new_pos = position + steps;
    if(new_pos < 0 || new_pos >= (int)array.size()){
        throw std::runtime_error("Failed to move tape: trying to move beyond the tape's borders.");
    }
    position = new_pos;

    usleep(Config::MOVE_DELAY_MS * 1000);
}

template<typename T>
void ArrayTape<T>::rewind(const size_t& new_pos){
    if(new_pos >= array.size()){
        throw std::runtime_error("Failed to move tape: trying to move beyond the tape's borders.");
    }
    position = new_pos;

    usleep(Config::REWIND_DELAY_MS * 1000);
}

template<typename T>
T ArrayTape<T>::read(){
    usleep(Config::READ_DELAY_MS * 1000);

    return array[position];
}

template<typename T>
void ArrayTape<T>::write(const T& data){
    usleep(Config::WRITE_DELAY_MS * 1000);

    array[position] = data;
}

template<typename T>
void ArrayTape<T>::print(){
    for(size_t i=0; i<array.size(); i++){
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}