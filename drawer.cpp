//
//  drawer.cpp
//  Visual Music
//
//  Created by Andrew Do on 2018-10-12.
//  Copyright © 2018 Andrew Do. All rights reserved.
//
#include <stdio.h>
#include "drawer.hpp"
#include <iostream>
#include "ResourcePath.hpp"


SPECTRO_GRAPH::SPECTRO_GRAPH(unsigned int num_bins,  unsigned int height_bins, unsigned int fft_size, unsigned int refresh_factor)
{
    char file_name[1024];
    
    std::cout << "select a .wav file from the \"/music\" folder : ";
    std::cin >> file_name;
    this->file_object = new FILE_INFO(file_name, fft_size, refresh_factor);
    
    this->fft_size = fft_size;
    this->scroll_x = 0;
    this->height_scale = 1024;
    unsigned int width = 1800;
    unsigned int height = 800;
    this->display = new sf::RenderWindow(sf::VideoMode(width, height), file_name);
    this->display->setFramerateLimit(60);
    this->width_total = width;
    this->height_total = height;
    this->num_bins = num_bins;
    this->height_bins = height_bins;
    this->width_graph = (width * 0.90);
    this->height_graph = (height * 0.80);
    this->bin_width = this->width_graph/((this->num_bins)*1.5);
    this->bin_height = this->height_graph/(this->height_bins*2);
    if(!this->font.loadFromFile(resourcePath() + "sansation.ttf"))
        std::cout << "ERORR: Font not loaded\n";
    this->finished = false;
        
    this->start = std::chrono::steady_clock::now();

}


void SPECTRO_GRAPH::draw_bin(unsigned int row_order, unsigned int stack_order, bool filled, sf::Color color)
{
    
    sf::RectangleShape box = sf::RectangleShape(sf::Vector2f(this->bin_width*1.25, this->bin_height));
    box.setPosition((float)row_order * this->bin_width * 1.5 + this->width_total * 0.05,
                    ((float)this->height_total * 0.84 - stack_order * this->bin_height * 2.45));
    
    if(filled) box.setFillColor(color);
    else box.setFillColor(sf::Color::Black);
    
    this->display->draw(box);

}


void SPECTRO_GRAPH::draw_bin_row(unsigned int row_order, float portion)
{
    bool filled;
    for(unsigned int i = 0; i < this->height_bins; i++)
    {
        if(this->height_scale * (float)i/this->height_bins < portion) filled = true;
        else filled = false;
        this->draw_bin(row_order, i, filled, sf::Color::White);
    }
}

unsigned int it = 0;
void SPECTRO_GRAPH::draw_spectro()
{
    this->file_object->sifter(scroll_x);

        sf::RectangleShape box = sf::RectangleShape(sf::Vector2f( this->width_graph, this->height_graph));
        box.setPosition(this->width_total * 0.05, this->height_total * 0.05);
        box.setOutlineThickness(this->width_total * 0.0025);
        box.setOutlineColor(sf::Color::Red);
        box.setFillColor(sf::Color::Black);
        this->display->draw(box);
        
        
        for(unsigned int i = 0; i < this->num_bins; i++)
        {
            this->draw_bin_row(i, this->file_object->output[i]);
            this->draw_freq(i);
        }
}


void SPECTRO_GRAPH::draw_graph()
{
    
    sf::Music music;
    if (!music.openFromFile(this->file_object->filename))
        return -1; // error
    music.play();
    
    while (this->display->isOpen() && !this->finished)
    {
        // Process events
        sf::Event event;
        while (this->display->pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                this->display->close();
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                this->display->close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
            {
                if(this->scroll_x <  (this->fft_size / 2) - this->num_bins) this->scroll_x++;
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
            {
                if(this->scroll_x > 0) this->scroll_x--;
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
            {
                this->height_scale = this->height_scale*2;
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
            {
                if(this->height_scale > 10) this->height_scale = this->height_scale/2;
            }
        }
        // Clear screen
        this->display->clear();
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point current;
        std::chrono::milliseconds interval((int)(round)(1000 * (double)this->file_object->time * (double)this->fft_size /(double)this->file_object->num_samples)/this->file_object->refresh_factor);
        
        // Draw
        this->draw_spectro();
        this->draw_time();
        
        // Update the window
        this->display->display();
        
        do
        {
            current = std::chrono::steady_clock::now();
        }while(std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count() < interval.count());
    }
}


void SPECTRO_GRAPH::draw_time()
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    int current = (int) std::chrono::duration_cast<std::chrono::seconds>(now - this->start).count();
    int seconds = current % 60;
    int minutes = current / 60;
    int total_seconds = (int)this->file_object->time % 60;
    int total_minutes = this->file_object->time / 60;
    std::string display_time("Time : " + std::to_string(minutes) + ":");
    
    if(seconds == total_seconds && minutes == total_minutes) this->finished = true;
    
    sf::Text text;

    if(seconds < 10)
            display_time.append("0");
        display_time.append(std::to_string(seconds) + " / " + std::to_string(total_minutes) + ":");
        if(total_seconds < 10)
            display_time.append("0");
        display_time.append(std::to_string(total_seconds));
        
        text.setString(display_time);
        text.setFont(this->font);
        text.setPosition(width_total*0.8, height_total*0);
        text.setFillColor(sf::Color::White);
        this->display->draw(text);
    
    std::string display_range("Vertical Range : " + std::to_string(this->height_scale));
    sf::Text text2;
    text2.setString(display_range);
    text2.setFont(this->font);
    text2.setPosition(width_total *0.05, height_total*0);
    text2.setFillColor(sf::Color::White);
    this->display->draw(text2);
    

}


void SPECTRO_GRAPH::draw_freq(unsigned int row_order)
{
    int freq = ((this->scroll_x + row_order) * this->file_object->sample_rate) / (file_object->num_bins);
    std::string freq_s(std::to_string(freq) + " Hz");
    sf::Text text;
    text.setString(freq_s);
    text.setFont(this->font);
    text.setRotation(45);
    text.setPosition((float)row_order * this->bin_width * 1.5 + this->width_total * 0.05 + this->bin_width, 1.075 * (float)this->height_graph);
    text.setFillColor(sf::Color::White);
    this->display->draw(text);
}


SPECTRO_GRAPH::~SPECTRO_GRAPH()
{
    delete this->display;
}
