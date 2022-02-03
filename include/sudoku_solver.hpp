#pragma once

#include <sudoku_utils.hpp>

namespace css
{   
    class sudoku_solver
    {        
        public:
            static constexpr inline uint32_t nrows{ 9U };
            static constexpr inline uint32_t ncols{ 9U };
            static constexpr inline uint32_t nvals{ 9U };
            static constexpr inline board_dimension dim{ nrows, ncols, nvals };
            
        public:
            sudoku_solver() = delete;

            sudoku_solver(const board& board)
                : m_board{ board }
            {
                sudoku_init_board(m_solver, dim);
                if (!sudoku_populate_solver(m_solver, m_board, dim))
                    throw std::runtime_error{ "[-] Error in populate solver" };
            }

            sudoku_solver(std::ifstream& input_file)
                : sudoku_solver{ parse_sudoku_file(input_file, dim) }
            {
            }
            
            bool solve() 
            {
                // TODO(garcia): Apply Rules Here?
                return m_solver.solve(); 
            }

            board get_model();

        private:
            board m_board;
            Minisat::Solver m_solver;

        private:
            inline void just_one_true(vec_literal const& literals);
            inline void one_value_per_quadrant();
            void non_double_values();// Implement in .cpp
    };

    inline void sudoku_solver::just_one_true(vec_literal const& literals)
    {
        m_solver.addClause(literals);

        for (decltype(literals.size()) i{0}; i < literals.size(); ++i)
            for (decltype(i) j = {i + 1}; j < literals.size(); ++j)
                m_solver.addClause(~literals[i], ~literals[j]);
    }

    inline void sudoku_solver::one_value_per_quadrant()
    {
        vec_literal literals;

        for (uint32_t row = 0U; row < nrows; ++row)
        {
            for (uint32_t col = 0U; col < ncols; ++col)
            {
                for (uint32_t val = 0U; val < nvals; ++val)
                    literals.push(Minisat::mkLit(make_unique_var(make_board_dim(row, col, val), 
                                                                 make_board_pos(row, col, val))));
                
                just_one_true(literals);
            }
        }
    }
}