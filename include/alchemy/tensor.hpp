#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace TENSOR {

template <class Tp_, int dimension>
struct Tensor {
    std::vector<int> dim; // Stores the dimensions of the tensor
    std::vector<Tp_> data;  // Stores the tensor data
    int size;

    // Tensor()
    Tensor();

    // Tensor(dim0, dim1, dim2 ...)
    template <class... Args>
    Tensor(Args&&... args);

    // to print tensor data
    void print() const;
};

// Tensor implementation
template <class Tp_, int dimension>
Tensor<Tp_, dimension>::Tensor() : dim(dimension, 1), data(1) {}

template <class Tp_, int dimension>
template <class... Args>
Tensor<Tp_, dimension>::Tensor(Args&&... args)
    : dim{static_cast<int>(std::forward<Args>(args))...}, data() {
    size = 1;
    for (auto& d : dim) size *= d;
    data.resize(size);
}

template <class Tp_, int dimension>
void Tensor<Tp_, dimension>::print() const {
    auto paint = [&](int x, std::string text) -> void {std::cout << "\033[1;" << std::to_string(x + 31) + "m" + text + "\033[0m";};
    std::cout << "dimension : " << dim.size() << "\n";
    std::cout << "index : ";
    for (int i = 0; i < dim.size(); i++) {
        paint(i, "[");
        std:: cout << dim.size() - i;
        paint(i, "] ");
    }
    std::cout << "\n";
    if (dim.size() == 0) {
        std::cout << "Emtpy Tensor\n";
        return ;
    }
    if (dim.size() == 1) {
        paint(0, "[");
        for (int i = 0; i < dim[0]; i++) {
            std::cout << data[0] << (i < dim[0] - 1 ? ", " : "");
        }
        paint(0, "]");
        return ;
    }
    auto tabs  = [&](int x) -> std::string { return std::string(x * 2, ' '); };
    auto print_recursive = [&](auto& self, int level, int idx, int size) -> void {
        // std::cout << "\033[1;" << std::to_string(level + 31) + "m";
        if (level == dim.size() - 2) {
                    //* dim == 2
            int row = dim[dim.size() - 2];
            int col = dim[dim.size() - 1];
            paint(level, tabs(level + 1) + "\b[");
            paint(level + 1, "[");
            for (int i = 0; i < row; i++, std::cout << "\n\0"[i == row]) {
                if (i != 0) paint(level + 1, tabs(level + 1) + "[");
                for (int j = 0; j < col; j++) {
                    std::cout << data[idx + col * i + j] << (j != col - 1 ? ", " : "");
                }
                paint(level + 1, "]");
            }
            paint(level, "]");
        } else {
                    //* dim >= 2
            paint(level, tabs(level) + "[\n");
            for (int i = 0; i < dim[level]; i++) {
                self(self, level + 1, idx + i * size / dim[level], size / dim[level]);
                std::cout << (i != dim[level] - 1 ? ",\n" : "");
                if (i != dim[level] - 1) std::cout << tabs(level + 1);
                std::cout << "\n";
            }
            paint(level, tabs(level) + "]");
        }
        // std::cout << "\033[0m";
    };
    std::cout << "\n";
    print_recursive(print_recursive, 0, 0, size);
    std::cout << "\n\n";
}

// Matrix definition and implementation
template <class Tp_, int xsize, int ysize>
struct Matrix : public Tensor<Tp_, 2> {
    // Default constructor
    Matrix() : Tensor<Tp_, 2>(xsize, ysize) {}

    // Constructor with elements
    template <class... Args>
    Matrix(Args&&... args)
        : Tensor<Tp_, 2>(xsize, ysize) {
        this->data = {static_cast<Tp_>(std::forward<Args>(args))...};
    }
};

// Vector definition and implementation
template <class Tp_, int size>
struct Vector : public Tensor<Tp_, 1> {
    // Default constructor
    Vector() : Tensor<Tp_, 1>(size) {}

    // Constructor with elements
    template <class... Args>
    Vector(Args&&... args)
        : Tensor<Tp_, 1>(size) {
        this->data = {static_cast<Tp_>(std::forward<Args>(args))...};
    }
};

} // namespace TENSOR