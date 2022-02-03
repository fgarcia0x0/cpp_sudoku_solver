#include <sudoku_solver.hpp>

namespace css
{
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