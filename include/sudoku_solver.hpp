#pragma once

#include <sudoku_utils.hpp>
#include <sudoku_rules.hpp>

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
                
                sudoku_apply_rule<sudoku_rule_types::one_value_per_sqr>{}(m_solver, dim);
                sudoku_apply_rule<sudoku_rule_types::one_value_per_col>{}(m_solver, dim);
                sudoku_apply_rule<sudoku_rule_types::one_value_per_row>{}(m_solver, dim);
                sudoku_apply_rule<sudoku_rule_types::one_value_per_quad>{}(m_solver, dim);

                if (!sudoku_populate_solver(m_solver, m_board, dim))
                    throw std::runtime_error{ "[-] Error in populate solver" };
            }

            sudoku_solver(std::ifstream& input_file)
                : sudoku_solver{ parse_sudoku_file(input_file, dim) }
            {
            }
            
            bool solve() 
            {
                return m_solver.solve(); 
            }

            board get_model();

        private:
            board m_board;
            Minisat::Solver m_solver;
    };
}