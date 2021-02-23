#pragma once

#include <vector>
#include <functional>
#include <cstdint>

namespace assfire::router {
    template<typename T>
    class Matrix {
    public:
        Matrix(std::size_t rows, std::size_t columns, std::function<T(int, int)> initial_value_supplier = [](int, int) { return T(); })
                : rows(rows), columns(columns),
                  data(rows * columns) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < columns; ++j) {
                    data.at(columns * i + j) = initial_value_supplier(i, j);
                }
            }
        }

        Matrix(const Matrix &rhs) = default;

        const T &at(int i, int j) const {
            return data.at(columns * i + j);
        }

        T &at(int i, int j) {
            return data.at(columns * i + j);
        }

        std::size_t getRowsCount() const {
            return rows;
        }

        std::size_t getColumnsCount() const {
            return columns;
        }

    private:
        std::vector<T> data;
        std::size_t rows;
        std::size_t columns;
    };
}