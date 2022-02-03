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
#include <tuple>
#include <minisat/core/Solver.h>

namespace css
{
    using board_row       = std::vector<int32_t>;
    using board           = std::vector<board_row>;
    using board_dimension = std::tuple<uint32_t, uint32_t, uint32_t>;
    using board_position  = std::tuple<uint32_t, uint32_t, uint32_t>;
    using vec_literal     = Minisat::vec<Minisat::Lit>;

    static inline board_position make_board_pos(uint32_t row, 
                                                uint32_t col, 
                                                uint32_t val)
    {
        return std::make_tuple(row, col, val);
    }

    static inline board_dimension make_board_dim(uint32_t rows, 
                                                 uint32_t cols, 
                                                 uint32_t vals)
    {
        return std::make_tuple(rows, cols, vals);
    }

    static inline Minisat::Var make_unique_var(board_dimension dim, 
                                               board_position pos)
    {
        auto [rows, cols, vals] = dim;
        auto [row, col, val] = pos;
        return row * cols * vals + col * vals + val;
    }
    
    static board parse_sudoku_file(std::istream& stream, board_dimension dim)
    {
        if (stream.eof())
            throw std::runtime_error{ "invalid file stream" };

        auto [rows, cols, _] = dim;
        board result(rows, board_row(cols));
        size_t nlines = {};

        for (std::string line; std::getline(stream, line) && nlines <= rows; )
        {
            if (line.empty())
                continue;
            
            // count whitespace characters
            size_t nspaces = std::count_if(line.cbegin(), line.cend(), [](char c){ return std::isspace(c); });

            size_t real_line_size = line.size() - nspaces;
            if (real_line_size != rows)
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

        if (nlines != rows)
            throw std::runtime_error{ "This file has missing line" };
        
        return result;
    }

    static inline board parse_sudoku_file(std::string_view filepath, 
                                          board_dimension dim)
    {
        std::ifstream stream{ filepath.data() };
        if (!stream)
            throw std::runtime_error{ "[-] Invalid file path" };

        return parse_sudoku_file(stream, std::move(dim));
    }

    // TODO(garcia): Apply FORCE_INLINE macro for this function
    static inline void sudoku_init_board(Minisat::Solver& solver, 
                                         board_dimension dim)
    {
        auto [nrows, ncols, nvals] = dim;
        for (uint32_t row = 0U; row < nrows; ++row)
            for (uint32_t col = 0U; col < ncols; ++col)
                for (uint32_t val = 0U; val < nvals; ++val)
                    static_cast<void>(solver.newVar());
    }

    static inline bool sudoku_populate_solver(Minisat::Solver& solver, 
                                              const board& board,
                                              board_dimension dim)
    {
        bool ret = true;
        auto [nrows, ncols, _] = dim;

        for (uint32_t row{}; row < nrows; ++row)
        {
            for (uint32_t col{}; col < ncols; ++col)
            {
                const auto& value = board[row][col];
                auto pos = make_board_pos(row, col, value - 1);

                if (value)
                {
                    auto variable = make_unique_var(dim, pos);
                    auto literal = Minisat::mkLit(std::move(variable));
                    ret &= solver.addClause(std::move(literal));
                }
            }
        }
        
        return ret;
    }

    static inline std::ostream& print_board(const board& input, 
                                            std::ostream& output, 
                                            std::string_view delim = " ")
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
