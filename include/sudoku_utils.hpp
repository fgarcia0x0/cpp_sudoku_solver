#pragma once

#include <fstream>
#include <istream>
#include <ostream>
#include <vector>
#include <cstdint>
#include <string_view>
#include <string>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <minisat/core/Solver.h>

namespace css
{
    using board_row = std::vector<int32_t>;
    using board = std::vector<board_row>;
    using board_dimension = std::pair<uint32_t, uint32_t>;

    static board parse_sudoku_file(std::istream& stream, board_dimension dim)
    {
        if (stream.eof())
            throw std::runtime_error{ "invalid file stream" };

        auto [n, m] = dim;
        board result(n, board_row(m));
        size_t nlines = {};

        for (std::string line; std::getline(stream, line) && nlines <= n; )
        {
            if (line.empty())
                continue;
            
            // count whitespace characters
            size_t nspaces = std::count_if(line.cbegin(), line.cend(), [](char c){ return std::isspace(c); });

            size_t real_line_size = line.size() - nspaces;
            if (real_line_size != n)
                throw std::runtime_error{ "Line #" + std::to_string(nlines) + " has invalid size" };

            size_t pos = {};

            for (char character : line)
            {
                if (std::isspace(static_cast<uint8_t>(character)))
                    continue;
                else if (std::isdigit(character))
                    result[nlines][pos++] = character - '0';
                else
                    throw std::runtime_error{ "Line #" + std::to_string(nlines) + "contains invalid character: '" + character + "'"};
            }

            ++nlines;
        }

        if (nlines != n)
            throw std::runtime_error{ "This file has missing line" };
        
        return result;
    }

    static inline board parse_sudoku_file(std::string_view filepath, board_dimension dim)
    {
        std::ifstream stream{ filepath.data() };
        if (stream)
            return parse_sudoku_file(stream, std::move(dim));
        else
            return {};
    }

    // TODO(garcia): Apply FORCE_INLINE macro for this function
    static inline void sudoku_init_board(Minisat::Solver& solver, uint32_t nrows,
                                         uint32_t ncols, uint32_t nvals)
    {
        for (uint32_t row = 0U; row < nrows; ++row)
            for (uint32_t col = 0U; col < ncols; ++col)
                for (uint32_t val = 0U; val < nvals; ++val)
                    static_cast<void>(solver.newVar());
    }

    static inline std::ostream& print_board(const board& input, std::ostream& output, std::string_view delim = " ")
    {        
        for (const auto& row : input)
        {
            for (const auto& col : row)
            {
                output << col << delim;
            }
            output << '\n';
        }

        return output;
    }

}
