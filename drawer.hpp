//
//  drawer.hpp
//  Visual Music
//
//  Created by Andrew Do on 2018-10-12.
//  Copyright © 2018 Andrew Do. All rights reserved.
//

#ifndef drawer_hpp
#define drawer_hpp

#include <stdio.h>
#include "w_reader.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <string>

class SPECTRO_GRAPH {
public:
    unsigned int
        width_graph,
        height_graph,
        width_total,
        height_total,
        num_bins,
        height_bins,
        fft_size;
    float *bins,
        bin_width,
        bin_height;
    sf::Color *color;
    sf::RenderWindow *display;
    FILE_INFO *file_object;
    
    unsigned int height_scale, scroll_x;
    
    std::chrono::steady_clock::time_point start;
    sf::Font font;
    bool finished;
    
    SPECTRO_GRAPH(unsigned int num_bins,  unsigned int height_bins, unsigned int fft_size, unsigned int refresh_factor);
    void draw_bin(unsigned int row_order, unsigned int stack_order, bool filled, sf::Color color);
    void draw_bin_row(unsigned int row_order, float portion);
    void draw_spectro();
    void draw_graph();
    void draw_time();
    void draw_freq(unsigned int row_order);
    ~SPECTRO_GRAPH();
};
#endif /* drawer_hpp */
