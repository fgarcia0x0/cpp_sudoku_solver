#include <sudoku_solver.hpp>

namespace css
{
    void sudoku_solver::non_double_values()
    {
        vec_literal literals;

        // In each row, for each value, forbid two column sharing that value
        for (uint32_t row { 0U }; row < nrows; ++row)
        {
            for (uint32_t val { 0U }; val < nvals; ++val)
            {
                for (uint32_t col{ 0U }; col < ncols; ++col)
                    literals.push(Minisat::mkLit(make_unique_var(make_board_dim(9U, 9U, 9U), 
                                                                 make_board_pos(row, col, val))));
                
                just_one_true(literals);
            }
        }

        literals.clear();

        // In each row, for each value, forbid two column sharing that value
        for (uint32_t col{ 0U }; col < ncols; ++col)
        {
            for (uint32_t val{ 0U }; val < nvals; ++val)
            {
                for (uint32_t row{ 0U }; row < nrows; ++row)
                    literals.push(Minisat::mkLit(make_unique_var(make_board_dim(9U, 9U, 9U), 
                                                                 make_board_pos(row, col, val))));
                
                just_one_true(literals);
            }
        }

        literals.clear();

        // Now forbid sharing in the 3x3 boxes
        for (uint32_t q_row{ 0U }; q_row < nrows; q_row += 3)
        {
            for (uint32_t q_col{ 0U }; q_col < ncols; q_col += 3)
            {
                for (uint32_t val{ 0U }; val < nvals; ++val)
                {
                    for (uint32_t i_row{ 0U }; i_row < 3U; ++i_row)
                        for (uint32_t i_col{ 0U }; i_col < 3U; ++i_col)
                            literals.push(Minisat::mkLit(make_unique_var(make_board_dim(9U, 9U, 9U), 
                                                                         make_board_pos(q_row + i_row,
                                                                                        q_col + i_col,
                                                                                        val))));
                    
                    just_one_true(literals);
                }
            }
        }
    }

    board sudoku_solver::get_model()
    {
        board result{ nrows, board_row{ ncols } };

        for (uint32_t row{}; row < nrows; ++row)
        {
            for (uint32_t col{}; col < ncols; ++col)
            {
                uint32_t nfound{};

                for (uint32_t val{}; val < nvals; ++val)
                {
                    auto pos = make_board_pos(row, col, val);
                    auto mval = m_solver.modelValue(make_unique_var(dim, pos));
                    
                    if (mval.isTrue())
                    {
                        ++nfound;
                        result[row][col] = val + 1;
                    }
                }

                if (nfound > 1)
                    throw std::runtime_error{ "[-] Rule Break: The solver assigned one position with more than one value" };
            }
        }

        return result;
    } 
}