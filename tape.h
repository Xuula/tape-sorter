#ifndef __TAPE_H__
#define __TAPE_H__

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <unistd.h>

#include "config.h"

namespace Tape{

template<typename T>
class Tape{
public:
    virtual size_t getSize() const = 0;

    virtual size_t getPosition() const = 0;
    virtual void move(const int& steps) = 0;
    virtual void rewind(const size_t& pos) = 0;

    virtual T read() = 0;
    virtual void write(const T& val) = 0;
};

enum Direction{
    FORWARDS  = 1,
    BACKWARDS = -1
};

template <typename T>
void copy(const std::vector<T> &values, Tape<T> *dest, Direction direction);

template <typename T>
void copy(Tape<T> *src, Direction read_dir, Tape<T> *dest, Direction write_dir, size_t num_vals);


template<typename T>
class FileTape: public Tape<T>{
public:
    FileTape(const std::string& filename);
    void close();
    size_t getSize() const override;
    size_t getPosition() const override;
    void move(const int& steps) override;
    void rewind(const size_t& new_pos) override;
    T read() override;
    void write(const T& data) override;
private:
    std::fstream file;
    size_t position;
    const size_t size;
    size_t file_capacity(const std::string& filename);
};

template<typename T>
class ArrayTape: public Tape<T>{
public:
    ArrayTape(size_t size);
    ArrayTape(const std::vector<T> &array);
    size_t getSize() const override;
    size_t getPosition() const override;
    void move(const int& steps) override;
    void rewind(const size_t& new_pos) override;
    T read() override;
    void write(const T& data) override;
    void print();
private:
    std::vector<T> array;
    size_t position;
};

#include "tape.hpp"

};


#endif