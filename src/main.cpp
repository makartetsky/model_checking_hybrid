/**
 * @file main.cpp
 * @brief Application entry point definition.
 * @author Vadim Alimguzhin <darth@darth.su>
 * @date 19.07.2009
 */

#include <iostream>
#include <sstream>
#include <cstdlib>

#include <getopt.h>

#include <xercesc/util/PlatformUtils.hpp>

#include "types.hpp"
#include "solver.hpp"

using namespace std;

using xercesc::XMLPlatformUtils;
using xercesc::XMLException;

#define APP_VERSION_STR "0.1" ///< Application version.

/**
 * @brief Prints help message.
 *
 * @param[in] filename
 */
void
usage(const char* filename)
{
  cout << "Prototype of model checker for hybrid systems " << APP_VERSION_STR        << endl;
  cout << "usage: " << filename << " [-v level] [-q value] filename "                << endl;
  cout << "       read model from \"filename\" and verify with \"value\" of q_param" << endl;
  cout << "   or: " << filename << " -h"                                             << endl;
  cout << "       print help message"                                                << endl;
}

/**
 * @brief Application entry point.
 *
 * @param[in] argc Arguments count.
 * @param[in] argv Arguments array.
 *
 * @return 0 if success, != 0 otherwise.
 */
int
main(int argc, char* argv[])
{
  string filename;
  // defaults
  int verbose = 1;
  int q_param = 4;

  static struct option options[] =
  {
    {"help", 0, 0, 'h'},
    {"verbose", 1, 0, 'v'},
    {"q_param", 1, 0, 'q'},
    {0, 0, 0, 0}
  };


  int opt_idx;
  int c;

  for (;;)
  {
    c = getopt_long(argc, argv, "q:v:h", options, &opt_idx);
    if (c == -1)
      break;

    switch (c)
    {
      case 'h':
        usage(argv[0]);
        return 1;

      case 'v':
        verbose = atoi(optarg);
        break;

      case 'q':
        q_param = atoi(optarg);
        break;

      case '?':
      default:
        return 1;
    }
  }

  if (optind == argc - 1)
  {
    filename = argv[optind];
  }
  else if (optind == argc)
  {
    cout << "filename is missing" << endl << endl;
    usage(argv[0]);
    return 1;
  }
  else if (optind < argc)
  {
    cout << "extra arguments" << endl << endl;
    usage(argv[0]);
    return 1;
  }

  try
  {
    XMLPlatformUtils::Initialize();
    mc_hybrid::Solver solver;
    solver.verify(filename, q_param, verbose);
    XMLPlatformUtils::Terminate();
  }
  catch (const exception& e)
  {
    cerr << e.what() << endl;
    XMLPlatformUtils::Terminate();
    exit(EXIT_FAILURE);
  }
  catch (const XMLException& e)
  {
    cerr << "Problems with XML library." << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
