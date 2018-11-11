#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "drawer.hpp"
#include "w_reader.hpp"


int main(int, char const**)
{
    SPECTRO_GRAPH *spectro = new SPECTRO_GRAPH(32, 128, 1024, 128);
    spectro->draw_graph();

    
    return EXIT_SUCCESS;
}
