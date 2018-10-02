#include <iostream>
#include <stdexcept>
#include "cli.hpp"

using namespace Sko;
using namespace std;

int main(int argc, const char ** argv)
{
    Cli * app = new Cli();
    if (argc == 2) {
        try {
            app->loadSave(argv[1]);
        } catch (std::runtime_error &e) {
            delete app;
            cerr << "Unable to load file `" << argv[1] << "'. ";
            cerr << e.what() << endl;
            return 1;
        } catch (std::range_error &e) {
            delete app;
            cerr << "Board coordinate look-up error: " << e.what() << endl;
            return 1;
        }
    }
    app->run();
    delete app;
    return 0;
}