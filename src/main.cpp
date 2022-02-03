#include <iostream>
#include <sudoku_utils.hpp>

int main(int argc, char** argv)
{
    std::string_view filepath{ "../samples/sample_1.txt" };
    css::board grid{ css::parse_sudoku_file(filepath, {9U, 9U}) };
    css::print_board(grid, std::cout);
}