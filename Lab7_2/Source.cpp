#include <random>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <windows.h>

const int matrix_size = 5;

std::stringstream PrintMatrix(const std::vector<std::vector<short>>& matrix);

DWORD ReplacingPositiveElement(LPVOID lpParameter);
DWORD CountElements(LPVOID lpParameter);

int main()
{
    // Making random mechanism
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(-9, 9);

    // Generate random matrix
    std::vector<std::vector<short>> matrix(matrix_size);
    std::generate(matrix.begin(), matrix.end(), [&]()
    {
        std::vector<short> vec(matrix_size);
        std::generate(vec.begin(), vec.end(), [&]()
        {
            return distrib(gen);
        });
        return vec;
    });

    std::cout << "Raw matrix:\n";
    std::cout << PrintMatrix(matrix).rdbuf();
    std::cout << std::endl;

    HANDLE count_elements
        = CreateThread(NULL, 0, CountElements, &matrix, 0, NULL);
    HANDLE replacing_positive_element
        = CreateThread(NULL, 0, ReplacingPositiveElement, &matrix, 0, NULL);

    HANDLE handle_array[2]{ replacing_positive_element, count_elements };
    WaitForMultipleObjects(2, handle_array, true, INFINITE);

    CloseHandle(replacing_positive_element);
    CloseHandle(count_elements);

    return 0;
}

std::stringstream PrintMatrix(const std::vector<std::vector<short>>& matrix)
{
    std::stringstream output_msg;
    for (const auto& rows : matrix)
    {
        for (const auto& iter : rows)
        {
            output_msg << std::setw(2) << iter << " ";
        }
        output_msg << std::endl;
    }

    return output_msg;
}

DWORD ReplacingPositiveElement(LPVOID lpParameter)
{
    std::vector<std::vector<short>>& matrix
        = *((std::vector<std::vector<short>>*)lpParameter);

    for (auto& row : matrix)
    {
        std::replace_if(row.begin(), row.end(),
            std::bind(std::greater_equal<short>(), std::placeholders::_1, 0), row.at(row.size() - 2)
        );
    }

    std::stringstream output_msg;
    output_msg << "Matrix after ReplacingPositiveElement function:\n";
    output_msg << PrintMatrix(matrix).rdbuf();
    output_msg << std::endl << std::endl;

    std::cout << output_msg.rdbuf();

    return 0;
}

DWORD CountElements(LPVOID lpParameter)
{
    const std::vector<std::vector<short>>& matrix
        = *((std::vector<std::vector<short>>*)lpParameter);

    std::stringstream output_msg;
    for (const auto& row : matrix)
    {
        bool isCoincidence = false;
        output_msg << "Row: ";
        for (int i = 0; i < row.size(); ++i)
        {
            if (i != row.size() - 2 && row.at(i) == row.at(row.size() - 2))
            {
                isCoincidence = true;
            }
            output_msg << std::setw(2) << row.at(i) << " ";
        }
        output_msg << "-> " << std::boolalpha << isCoincidence << std::endl;
    }
    output_msg << std::endl << std::endl;
    std::cout << output_msg.rdbuf();

    return 0;
}
