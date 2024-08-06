#pragma once
#include <iostream>
#include <vector>

namespace MATRIX {

typedef int Index;

// Matrix (Extended Dimension)
template <class Tp_, int dimension>
struct MatrixEX {
    std::vector<Index> dim; // Stores the dimensions of the matrix
    std::vector<Tp_> data;  // Stores the matrix data

    MatrixEX();

    // Variadic template constructor to initialize dimensions
    template <class... Args>
    MatrixEX(Args&&... args);

    // Debug function to print matrix data
    void debug() const;
};

// Reference (Placeholder, add your implementation details)
template <class Tp_>
struct Ref {
    // Add members and methods as needed
};

// Vector (2D Matrix)
template <class Tp_>
struct Vector : MatrixEX<Tp_, 2> {
    Vector();
    Vector(Index x_size, Index y_size);
    ~Vector();
};

// Matrix (2D Matrix with compile-time dimensions)
template <class Tp_, int x_size, int y_size>
struct Matrix : MatrixEX<Tp_, 2> {
    Matrix();
};

// List (1D Matrix)
template <class Tp_, int size>
struct List : MatrixEX<Tp_, 1> {
    List();
};


















// MatrixEX implementation
template <class Tp_, int dimension>
MatrixEX<Tp_, dimension>::MatrixEX() = default;

template <class Tp_, int dimension>
template <class... Args>
MatrixEX<Tp_, dimension>::MatrixEX(Args&&... args)
    : dim{std::forward<Args>(args)...}, data() {
    int total_size = 1;
    for (auto& d : dim) total_size *= d;
    data.resize(total_size);
}

template <class Tp_, int dimension>
void MatrixEX<Tp_, dimension>::debug() const {
    auto print_recursive = [&](int level) {
        // Implementation for recursive printing
    };
}

} // namespace MATRIX
