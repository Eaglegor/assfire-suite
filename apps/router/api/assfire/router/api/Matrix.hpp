#pragma once

#include <vector>
#include <functional>
#include <cstdint>

namespace assfire::router
{
    template<typename T>
    class Matrix
    {
    public:
        Matrix(std::size_t rows, std::size_t columns, std::function<T(int, int)> initial_value_supplier = [](int, int) { return T(); })
                : rows(rows), columns(columns),
                  data(rows * columns) {
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < columns; ++j) {
                    data[columns * i + j] = initial_value_supplier(i, j);
                }
            }
        }

        Matrix(const Matrix &rhs) = default;

        Matrix(Matrix &&rhs) noexcept = default;

        Matrix &operator=(const Matrix &rhs) = default;

        Matrix &operator=(Matrix &&rhs) noexcept = default;

        const T &at(int i, int j) const {
            return data[columns * i + j];
        }

        void set(int i, int j, T value) {
            data[columns * i + j] = std::move(value);
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