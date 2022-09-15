#include "Matrix.hpp"

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


bool Matrix::MulRow(const float& scalar, const uint32_t rowNumber) {
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

    float* swapThing = array_[row1];
    array_[row1] = array_[row2];
    array_[row2] = swapThing;

    return 0;
}


bool Matrix::SwapColumns(const uint32_t column1, const uint32_t column2, Matrix& matrix2) {
    if ((GetRows() != matrix2.GetRows()) || (column1 >= GetColumns()) || (column2 >= GetColumns())) {
        return 1;
    }

    for (uint32_t curRow = 0; curRow < GetRows(); curRow++) {
        float swapThing = array_[curRow][column1];
        array_[curRow][column1] = matrix2.array_[curRow][column2];
        matrix2.array_[curRow][column2] = swapThing;
    }

    return 0;
}


bool Matrix::AddMuledRow(const uint32_t rowToAdd, const uint32_t rowFromAdd, const float& scalar) {
    if ((rowToAdd >= GetRows()) || (rowFromAdd >= GetRows())) {
        return 1;
    }

    for (uint32_t curColumn = 0; curColumn < GetColumns(); curColumn++) {
        array_[rowToAdd][curColumn] += scalar * array_[rowFromAdd][curColumn];
    }

    return 0;
}


float Matrix::CalculateDeterminant() {
    if ((GetColumns() != GetRows()) || (GetRows() == 0)) {
        return NAN;
    }

    float answer = 1;

    for (uint32_t curRow = 0; (curRow < GetRows()) && !CmpFloat(answer, 0); curRow++) {
        float divider = array_[curRow][curRow];

        if (CmpFloat(divider, 0)) {
            for (uint32_t curNextRow = curRow + 1; curNextRow < GetRows(); curNextRow++) {
                if (!CmpFloat(array_[curNextRow][curRow], 0)) {
                    SwapRows(curNextRow, curRow);
                    divider = array_[curRow][curRow];

                    answer = -answer;
                    break;
                }
            }
        }
        answer *= divider;

        for (uint32_t curNextRow = curRow + 1; curNextRow < GetRows(); curNextRow++) {
            float curMul    = -array_[curNextRow][curRow] / divider;

            AddMuledRow(curNextRow, curRow, curMul);
        }
    }

    return answer;
}
