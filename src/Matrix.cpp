#include "Matrix.hpp"

Matrix& Matrix::operator=(const Matrix& cloneMatrix) {
    if (this == &cloneMatrix) {
        return *this;
    }

    this->~Matrix();
    array_ = new double*[cloneMatrix.GetRows()];

    rowsAmount_   = cloneMatrix.rowsAmount_; 
    columnAmount_ = cloneMatrix.columnAmount_;

    for (uint32_t curRow = 0; curRow < rowsAmount_; curRow++) {
        array_[curRow] = new double[columnAmount_];

        for (uint32_t curColumn = 0; curColumn < columnAmount_; curColumn++) {
            array_[curRow][curColumn] = cloneMatrix.array_[curRow][curColumn];
        }
    }

    return *this;
}

bool Matrix::SetRow(const uint32_t thisRowNumber, const uint32_t otherRowNumber, const Matrix& secondMAtrix) {
    if ((GetColumns() != secondMAtrix.GetColumns()) || 
        (thisRowNumber >= GetRows()) || (otherRowNumber >= secondMAtrix.GetRows())) {
        return 1;
    }

    for (uint32_t curColumn = 0; curColumn < columnAmount_; curColumn++) {
        array_[thisRowNumber][curColumn] = secondMAtrix.array_[otherRowNumber][curColumn];
    }

    return 0;
}

bool Matrix::AddRow(const uint32_t rowToAdd, const uint32_t rowFromAdd) {
    if ((rowToAdd >= GetRows()) || (rowFromAdd >= GetRows())) {
        return 1;
    }

    for (uint32_t curColumn = 0; curColumn < GetColumns(); curColumn++) {
        array_[rowToAdd][curColumn] += array_[rowFromAdd][curColumn];
    }

    return 0;
}


bool Matrix::AddRow(const Matrix& rowToAdd, const uint32_t rowNumber) {
    if ((rowToAdd.GetRows() != 1) || (GetColumns() != rowToAdd.GetColumns())) {
        return 1;
    }
    
    for (uint32_t curColumn = 0; curColumn < GetColumns(); curColumn++) {
        array_[rowNumber][curColumn] += rowToAdd.array_[0][curColumn];
    }

    return 0;
}


bool Matrix::MulRow(const double& scalar, const uint32_t rowNumber) {
    if (rowNumber >= GetRows()) {
        return 1;
    }

    for (uint32_t curColumn = 0; curColumn < GetColumns(); curColumn++) {
        array_[rowNumber][curColumn] *= scalar;
    }

    return 0;
}


bool Matrix::SwapRows(const uint32_t row1, const uint32_t row2) {
    if ((row1 >= GetRows()) || (row2 >= GetRows())) {
        return 1;
    }

    double* swapThing = array_[row1];
    array_[row1] = array_[row2];
    array_[row2] = swapThing;

    return 0;
}


bool Matrix::SwapColumns(const uint32_t column1, const uint32_t column2, Matrix& matrix2) {
    if ((GetRows() != matrix2.GetRows()) || (column1 >= GetColumns()) || (column2 >= matrix2.GetColumns())) {
        return 1;
    }

    for (uint32_t curRow = 0; curRow < GetRows(); curRow++) {
        double swapThing = array_[curRow][column1];
        array_[curRow][column1] = matrix2.array_[curRow][column2];
        matrix2.array_[curRow][column2] = swapThing;
    }

    return 0;
}


bool Matrix::AddMuledRow(const uint32_t rowToAdd, const uint32_t rowFromAdd, const double& scalar) {
    if ((rowToAdd >= GetRows()) || (rowFromAdd >= GetRows())) {
        return 1;
    }

    for (uint32_t curColumn = 0; curColumn < GetColumns(); curColumn++) {
        array_[rowToAdd][curColumn] += scalar * array_[rowFromAdd][curColumn];
    }

    return 0;
}


double Matrix::CalculateDeterminant() const {
    if ((GetColumns() != GetRows()) || (GetRows() == 0)) {
        return NAN;
    }

    Matrix copyMatrix(*this);
    double answer = 1;

    for (uint32_t curRow = 0; (curRow < GetRows()) && !CmpDbl(answer, 0); curRow++) {
        double divider = copyMatrix.array_[curRow][curRow];

        if (CmpDbl(divider, 0)) {
            for (uint32_t curNextRow = curRow + 1; curNextRow < GetRows(); curNextRow++) {
                if (!CmpDbl(copyMatrix.array_[curNextRow][curRow], 0)) {
                    copyMatrix.SwapRows(curNextRow, curRow);
                    divider = copyMatrix.array_[curRow][curRow];

                    answer = -answer;
                    break;
                }
            }
        }
        answer *= divider;

        for (uint32_t curNextRow = curRow + 1; curNextRow < GetRows(); curNextRow++) {
            double curMul    = -copyMatrix.array_[curNextRow][curRow] / divider;

            copyMatrix.AddMuledRow(curNextRow, curRow, curMul);
        }
    }

    return answer;
}

std::ostream& operator<<(std::ostream& outStream, const Matrix& matrix) {
    for (uint32_t curRow = 0; curRow < matrix.GetRows(); curRow++) {
        for (uint32_t curColumn = 0; curColumn < matrix.GetColumns(); curColumn++) {
            outStream << matrix.GetElem(curRow, curColumn) << " ";
        }
        outStream << std::endl;
    }

    return outStream;
}
