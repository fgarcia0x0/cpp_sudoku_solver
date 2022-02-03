#pragma once

#include <sudoku_utils.hpp>

namespace css
{   
    class sudoku_solver
    {        
        public:
            static constexpr inline uint32_t nrows = 9U;
            static constexpr inline uint32_t ncols = 9U;
            static constexpr inline uint32_t nvals = 9U;
            
        public:
            sudoku_solver() = delete;

            sudoku_solver(std::ifstream& input_file)
                : m_board{ parse_sudoku_file(input_file, board_dimension{nrows, ncols}) }
            {
            }
            
            sudoku_solver(const board& board)
                : m_board{ board }
            {
                sudoku_init_board(m_solver, nrows, ncols, nvals);
            }

            bool solve() noexcept;
            board get_model() noexcept;

        private:
            board m_board;
            Minisat::Solver m_solver;
        private:
            // functions
    };
}