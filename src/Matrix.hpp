#pragma once

#include <iostream>
#include <cstdint>
#include <cassert>
#include <algorithm>

#include "Utilities.hpp"

class Matrix {
    private:
        double** array_;

        uint32_t rowsAmount_   = 0;
        uint32_t columnAmount_ = 0;

    public:
        Matrix(const Matrix& cloneMatrix) : array_(nullptr) {
            rowsAmount_   = cloneMatrix.rowsAmount_;
            columnAmount_ = cloneMatrix.columnAmount_;
            if (!rowsAmount_ && !columnAmount_) {
                return;
            }

            array_        = new double*[cloneMatrix.GetRows()];
            for (uint32_t curRow = 0; curRow < rowsAmount_; curRow++) {
                array_[curRow] = new double[columnAmount_];

                for (uint32_t curColumn = 0; curColumn < columnAmount_; curColumn++) {
                    array_[curRow][curColumn] = cloneMatrix.array_[curRow][curColumn];
                }
            }
        }

        Matrix& operator=(const Matrix& cloneMatrix);

        Matrix(const uint32_t rows = 1, const uint32_t columns = 1) :
            array_(new double*[rows]),
            rowsAmount_(rows), columnAmount_(columns) 
        {
            for (uint32_t curRow = 0; curRow < rows; curRow++) {
                array_[curRow] = new double[columns];
            }
        }

        uint32_t GetColumns() const {
            return columnAmount_;
        }

        uint32_t GetRows() const {
            return rowsAmount_;
        }

        bool SetRow(const uint32_t thisRowNumber, const uint32_t otherRowNumber, const Matrix& secondMAtrix);

        bool AddRow(const Matrix& rowToAdd, const uint32_t rowNumber);
        bool AddRow(const uint32_t row1, const uint32_t row2);

        bool MulRow(const double& scalar, const uint32_t rowNumber);
        bool SwapRows(const uint32_t row1, const uint32_t row2);

        bool SwapColumns(const uint32_t column1, const uint32_t column2, Matrix& matrix2);

        double CalculateDeterminant() const;

        double GetElem(const uint32_t curRow, const uint32_t curColumn) const {
            if ((curRow >= rowsAmount_) || (curColumn >= columnAmount_)) {
                return NAN;
            }
            
            return array_[curRow][curColumn];
        }

        bool SetElem(const uint32_t curRow, const uint32_t curColumn, const double& elem) {
            if ((curRow >= rowsAmount_) || (curColumn >= columnAmount_)) {
                return 1;
            }

            array_[curRow][curColumn] = elem;

            return 0;
        }

        bool AddMuledRow(const uint32_t rowToAdd, const uint32_t rowFromAdd, const double& scalar);

        ~Matrix() {
            for (uint32_t curRow = 0; curRow < rowsAmount_; curRow++) {
                delete[] array_[curRow];
            }

            delete[] array_;
        }  
};

std::ostream& operator<<(std::ostream& outStream, const Matrix& matrix);
