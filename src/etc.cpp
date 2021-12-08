#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <array>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <fstream>
#include <sstream>

// Argument loading
#include <getopt.h>
// GLUT
#include <GL/glut.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) // Windows
    #define NEWLINE "\r\n"
    #include <windows.h> 
#else
    #define NEWLINE '\n' // Avoid std::endl because of proper buffering and flushing
#endif

// Global variabiable map for callback GLUT function
int64_t MAX_SIZE {1000},
        h {0},
        w {0},
        radius_global {2}, // Radius of spread
        //v_infectious_tick {};
        map_size {400};

// Mouse events
bool  leftMouseButtonDown { false },
      rightMouseButtonDown { false };
int64_t mouseXPos {0},
        mouseYPos {0};

void mouse(int button, int state, int x, int y)
{
    // Save the button state
    if (button == GLUT_LEFT_BUTTON)
      { leftMouseButtonDown = (state == GLUT_DOWN); }

    else if (button == GLUT_RIGHT_BUTTON)
      { rightMouseButtonDown = (state == GLUT_DOWN); }

    // Save the mouse position
    mouseXPos = (int64_t)y;
    mouseYPos = (int64_t)x;
}

// Logging
bool logging {false};
std::ofstream map_state_log;
int64_t log_period {25},
        tick {0},
        active {0},

        population {10000},
        
        vaccinated {0},
        partially_vaccinated {0},
        nonvaccinated {0},

        healthy {0},
        infected {0},
        dead {0},
        retrieved {0},

        v_healthy {0},
        v_infected {0},
        v_dead {0},
        v_retrieved {0},

        p_healthy {0},
        p_infected {0},
        p_dead {0},
        p_retrieved {0},

        n_healthy {0},
        n_infected {0},
        n_dead {0},
        n_retrieved {0};

std::ofstream initLog(std::string filename)
{
  std::ofstream outfile;
  outfile.open(filename, std::ofstream::out | std::ios_base::trunc); // append instead of overwrite
  outfile << "";
  return outfile;
}

void logAdd(std::ofstream outfile, std::string out)
{
  if (outfile.is_open())
  {
    outfile << out;
  }
}

// Enumerate implementation
template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable)
{
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

// Function for help text
void showHelp()
{
  std::cout
    << "Usage:" << NEWLINE
    << "  ./ims [-h|--help] [-g|--gui] " << NEWLINE
    << NEWLINE
    << "Flags:" << NEWLINE
    << "  -h, --help\t" << "Show help" << NEWLINE
    << "  -g, --gui\t"  << "GUI version" << NEWLINE
    << "  -x X -y Y\t"  << "Set a point of infection origin" << NEWLINE
    << "  -l, --log\t"  << "Enable logging to a output.txt file" << std::endl;
  ;
}

// Argument parsing
void parseArgs(int& argc, 
              char* argv[], 
              bool& gui, 
              bool& show_help, 
              bool& load_map, 
              int64_t& x, 
              int64_t& y)
{  
  const struct option long_options[] =
  {
    {"gui",            no_argument, nullptr, 'g'},
    {"log",            no_argument, nullptr, 'l'},
    {"help",           no_argument, nullptr, 'h'},
    {"map-size",       no_argument, nullptr, 's'},
    {"population",     no_argument, nullptr, 'n'},
    {"vaccinated",     no_argument, nullptr, 'v'},
    {"part-vaccinated",no_argument, nullptr, 'p'},
    {nullptr,          no_argument, nullptr, 0}
  };

  int arg {0};
  // Load arguments
  while (arg != -1)
  {
    arg = getopt_long(argc, argv, "xyglhsnvp", long_options, nullptr);
    switch (arg)
    {
      case 'x':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
          { 
            x = val;
          }
        }
        else
        {
          std::cerr << "Please specify x coordinate." << std::endl;
          std::exit(EXIT_FAILURE);
        }

        break;

      case 'y':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
          { 
            y = val;
          }
        }
        else
        {
          std::cerr << "Please specify y coordinate." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

      case 's':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
          { 
            map_size = val;
          }
        }
        else
        {
          std::cerr << "Please specify map size." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

      case 'n':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
          { 
            population = val; 
          }
        }
        else
        {
          std::cerr << "Please specify population number." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

      case 'v':
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
            { vaccinated = val; }
        }
        else
        {
          std::cerr << "Please specify number of vaccinated people." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;
        
        case 'p':
        std::cout << argv[optind][0];
        if (!optarg and argv[optind] != nullptr and argv[optind][0] != '-')
        { 
          std::istringstream iss (argv[optind++]);
          int64_t val;

          if (iss >> val)
            { partially_vaccinated = val; }
        }
        else
        {
          std::cerr << "Please specify number of partially vaccinated people." << std::endl;
          std::exit(EXIT_FAILURE);
        }
        break;

      case 'g':
        gui = true;
        break;

      case 'l':
        logging = true;
        break;

      case 'h':
      case '?':
        show_help = true;
        break;

      default:
        //std::cerr << "Error! Enter all required arguments." << std::endl;
        //std::exit(EXIT_FAILURE);
        break;
    }
  }

}
