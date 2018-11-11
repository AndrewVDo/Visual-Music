//
//  w_reader.hpp
//  Visual_Music
//
//  Created by Andrew Do on 2018-10-10.
//  Copyright © 2018 Andrew Do. All rights reserved.
//
#pragma once
#ifndef w_reader_hpp
#define w_reader_hpp

#include "kiss_fft.h"
#include "ResourcePath.hpp"

#define TRUE 1
#define FALSE 0



#endif /* w_reader_hpp */

class FILE_INFO
{
public:
    unsigned char riff[4];                  //RIFF
    unsigned int overall_size;              //size of file in bytes
    unsigned char wave[4];                  //WAVE
    unsigned char format[4];                //Format chunk marker
    unsigned int format_size;               //length of format data
    unsigned short int format_type;         //1 for pcm, 2 for int
    unsigned short int channels;            //number of channels
    unsigned int sample_rate;               //CD = 44100, DAT = 48000, Hz
    unsigned int byte_rate;                 //sample_rate*bits_per_sample*channels/8
    unsigned short int block_align;         //bits_per_sample*channels/8.1-8
    unsigned short int bits_per_sample;     //
    unsigned char data[4];                  //data chunk HEADER
    unsigned int data_size;                 //size of next data chunk to be read
    
    FILE *ptr;
    char *filename;
    
    double time;
    unsigned int num_samples;
    unsigned int sample_needle;
    kiss_fft_cpx *samples;
    unsigned int refresh_factor;
    
    unsigned int num_bins;
    kiss_fft_cpx *bins;
    kiss_fft_cfg fft_config;
    kiss_fft_scalar *output;
    
    
    FILE_INFO(char* filename, unsigned int num_bins, unsigned int refresh_factor);
    int w_reader(char *input);
    void print_header();
    void sifter(unsigned int scroll_x);
    ~FILE_INFO();
};

