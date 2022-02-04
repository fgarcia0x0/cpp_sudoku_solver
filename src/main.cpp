#include <iostream>
#include <chrono>
#include <sudoku_solver.hpp>

#if defined(_WIN32) || defined(WIN32) 
    #define CSS_PREFERRED_SLASH '\\'
#else
    #define CSS_PREFERRED_SLASH '/'
#endif

using std::chrono::duration;
using std::chrono::high_resolution_clock;

inline std::ostream& operator <<(std::ostream& output, const css::board& board)
{
    return css::print_board(board, output);
}

inline void print_usage(std::string_view prog_name)
{
    std::cout << "[!] Usage: " << prog_name << " [sudoku_files]\n\n";
}

int main(int argc, char** argv)
{
    std::string_view program_name{ argv[0] };
    bool flag_only_model = false;

    --argc, ++argv;
     
    if (argc && argv)
    {
        size_t nargs{ static_cast<size_t>(argc) };

        for (size_t index = 0; index < nargs; ++index)
        {
            if (!std::strcmp(argv[index], "--only-model"))
            {
                flag_only_model = true;
                continue;
            }

            std::ifstream input_stream{ argv[index] };
            const char* target = std::strrchr(argv[index], CSS_PREFERRED_SLASH);
            
            if (target)
                ++target;

            if (input_stream)
            {
                try
                {
                    auto t0{ high_resolution_clock::now() };
                    css::sudoku_solver solver{ input_stream };

                    if (solver.solve())
                    {
                        css::board sudoku_solution{ solver.get_model() };
                        
                        if (!flag_only_model)
                        {
                            auto t1{ high_resolution_clock::now() };
                            duration<double, std::milli> elapsed{ t1 - t0 };

                            std::cout << "[+] Solution " << "for \"" << target
                                      << "\" found in "  << elapsed.count() << " ms\n\n";
                        }

                        std::cout << sudoku_solution << '\n';
                    }
                    else
                    {
                        std::cerr << "[-] Impossible to solve this sudoku\n";
                        return EXIT_FAILURE;
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "[-] Cannot resolve \"" << target 
                              << "\" instance\n"
                              << e.what() << "\n\n";
                    continue;
                }
            }
            else
            {
                std::cerr << "[-] Invalid filepath argument [likely file path not exists]" << '\n';
                return EXIT_FAILURE;
            }
        }

        return EXIT_SUCCESS;
    }
    else
    {
        print_usage(program_name);
        return EXIT_FAILURE;
    }
}