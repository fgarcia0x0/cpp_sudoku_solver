#pragma once

#include <memory>
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

            template <typename TBoard, 
                      typename = std::enable_if_t<std::is_same_v<std::decay_t<TBoard>, board>>>
            sudoku_solver(TBoard&& board)
                : m_board{ std::forward<TBoard>(board) }
            {
                m_solver = std::make_shared<sat_solver>();
                sudoku_prepare();
            }

            sudoku_solver(std::ifstream& input_file)
                : sudoku_solver{ parse_sudoku_file(input_file, dim) }
            {
            }
            
            bool solve()
            {
                return m_solver->solve();
            }

            board get_model();

            template <typename TBoard, 
                      typename = std::enable_if_t<std::is_same_v<std::decay_t<TBoard>, board>>>
            void reset(TBoard&& new_board)
            {
                m_board = std::forward<TBoard>(new_board);
                m_solver.reset(new sat_solver{});
                sudoku_prepare();
            }

        private:
            board m_board;
            std::shared_ptr<sat_solver> m_solver;
        private:
            inline void sudoku_prepare();
    };

    inline void sudoku_solver::sudoku_prepare()
    {
        sudoku_init_board(m_solver, dim);
                
        sudoku_apply_rule<sudoku_rule_types::one_value_per_sqr>{}(m_solver, dim);
        sudoku_apply_rule<sudoku_rule_types::one_value_per_col>{}(m_solver, dim);
        sudoku_apply_rule<sudoku_rule_types::one_value_per_row>{}(m_solver, dim);
        sudoku_apply_rule<sudoku_rule_types::one_value_per_quad>{}(m_solver, dim);

        if (!sudoku_populate_solver(m_solver, m_board, dim))
            throw std::runtime_error{ "[-] Error in populate solver" };
    }
}