#pragma once

#include <cassert>
#include <type_traits>
#include <sudoku_utils.hpp>

// Anonymous Namespace
namespace
{
    // ensure exactly one of this set of literals has to evaluate to true
    template <typename T, 
              typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, css::vec_literal>>>
    inline void ensure_just_one_true(css::sat_solver& solver, T&& literals)
    {
        size_t size = static_cast<size_t>(literals.size());
        assert(size != SIZE_MAX);

        solver.addClause(literals);

        for (size_t i{}; i < size; ++i)
            for (size_t j{ i + 1 }; j < size; ++j)
                solver.addClause(~literals[i], ~literals[j]);
    }
}

namespace css
{
    enum class sudoku_rule_types : uint8_t
    {
        one_value_per_row,  // rule that guarantees only one value per line
        one_value_per_col,  // rule that guarantees only one value per column
        one_value_per_quad, // rule that guarantees only one value per quadrant
        one_value_per_sqr,  // rule that guarantees only one value per square
    };

    template <sudoku_rule_types RuleType>
    struct sudoku_apply_rule
    {
    };

    template <>
    struct sudoku_apply_rule<sudoku_rule_types::one_value_per_row>
    {
        void operator()(sat_solver& solver, board_dimension dim) const
        {
            auto [nrows, ncols, nvals] = dim;

            for (uint32_t col{}; col < ncols; ++col)
            {
                for (uint32_t val{}; val < nvals; ++val)
                {
                    vec_literal literals;

                    for (uint32_t row{}; row < nrows; ++row)
                    {
                        auto pos{ make_board_pos(row, col, val) };
                        literals.push(Minisat::mkLit(make_unique_var(dim, pos)));
                    }
                    
                    ensure_just_one_true(solver, std::move(literals));
                }
            }
        }
    };

    template <>
    struct sudoku_apply_rule<sudoku_rule_types::one_value_per_col>
    {
        void operator()(sat_solver& solver, board_dimension dim) const
        {
            auto [nrows, ncols, nvals] = dim;

            for (uint32_t row{}; row < nrows; ++row)
            {
                for (uint32_t val{}; val < nvals; ++val)
                {
                    vec_literal literals;

                    for (uint32_t col{}; col < ncols; ++col)
                    {
                        auto pos{ make_board_pos(row, col, val) };
                        literals.push(Minisat::mkLit(make_unique_var(dim, pos)));
                    }

                    ensure_just_one_true(solver, std::move(literals));
                }
            }
        }
    };

    template <>
    struct sudoku_apply_rule<sudoku_rule_types::one_value_per_quad>
    {
        void operator()(sat_solver& solver, board_dimension dim) const
        {
            auto [nrows, ncols, nvals] = dim;
            auto quad_size = static_cast<uint32_t>(std::sqrt(nrows));

            for (uint32_t r0{}; r0 < nrows; r0 += quad_size)
            {
                for (uint32_t c0{}; c0 < ncols; c0 += quad_size)
                {
                    for (uint32_t val{}; val < nvals; ++val)
                    {
                        vec_literal literals;

                        for (uint32_t r1{}; r1 < quad_size; ++r1)
                        {
                            for (uint32_t c1{}; c1 < quad_size; ++c1)
                            {
                                auto pos{ make_board_pos(r0 + r1, c0 + c1, val) };
                                literals.push(Minisat::mkLit(make_unique_var(dim, pos)));
                            }
                        }
                        
                        ensure_just_one_true(solver, std::move(literals));
                    }
                }
            }
        }
    };

    template <>
    struct sudoku_apply_rule<sudoku_rule_types::one_value_per_sqr>
    {
        void operator()(sat_solver& solver, board_dimension dim) const
        {
            auto [nrows, ncols, nvals] = dim;

            for (uint32_t row{}; row < nrows; ++row)
            {
                for (uint32_t col{}; col < ncols; ++col)
                {
                    vec_literal literals;
                    
                    for (uint32_t val{}; val < nvals; ++val)
                    {
                        auto pos{ make_board_pos(row, col, val) };
                        literals.push(Minisat::mkLit(make_unique_var(dim, pos)));
                    }
                    
                    ensure_just_one_true(solver, std::move(literals));
                }
            }
        }
    };

}