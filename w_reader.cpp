//
//  w_reader.cpp
//  Visual_Music
//
//  Created by Andrew Do on 2018-10-10.
//  Copyright © 2018 Andrew Do. All rights reserved.
//

#include "w_reader.hpp"
#include "kiss_fft.h"
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>


FILE_INFO::FILE_INFO(char* input, unsigned int num_bins, unsigned int refresh_factor)
{
    this->refresh_factor = refresh_factor;
    this->num_bins = num_bins;
    this->bins = (kiss_fft_cpx*) malloc(num_bins * sizeof(kiss_fft_cpx));
    this->fft_config = kiss_fft_alloc(this->num_bins, 0, NULL, NULL);
    this->sample_needle = 0;
    this->output = (kiss_fft_scalar*) malloc(this->num_bins * sizeof(kiss_fft_scalar) / 2);
    if(input != NULL)
    {
        this->w_reader(input);
    }
}



void FILE_INFO::sifter(unsigned int scroll_x)
{
    if(this->sample_needle < this->num_samples - (this->num_bins - 1))
    {
        kiss_fft(this->fft_config, &this->samples[this->sample_needle], this->bins);
        this->sample_needle += (this->num_bins / this->refresh_factor);
        for(int i = 0; i < this->num_bins / 2; i++)
        {
            this->output[i] = sqrt(this->bins[i + scroll_x].r * this->bins[i + scroll_x].r + this->bins[i + scroll_x].i * this->bins[i + scroll_x].i)/this->num_bins;
        }
    }
    else{
        for(int i = 0; i < this->num_bins / 2; i++)
        {
            this->output[i] = 0;
        }
    }
}



int FILE_INFO::w_reader(char *input)
{
    unsigned char buffer4[4];
    unsigned char buffer2[2];

    this->filename = (char*) malloc(sizeof(char) * 1024);
    if(this->filename == NULL) {
        printf("Error in malloc filename\n");
        exit(1);
    }
    std::string cwd("/Users/andrewdo/desktop/Visual_Music/Visual_Music"); // resource path not good, use absolute path
    strcpy(this->filename, cwd.c_str());
    strcat(this->filename, "/music/");
    strcat(this->filename, input);
    std::cout << "Music File Path : " << this->filename << "\n";

    this->ptr = fopen(this->filename, "rb");
    
    if(ptr == NULL) {
        printf("Error opening source file\n");
        exit(1);
    }
    
    std::size_t nread = 0;
    nread = fread(this->riff, sizeof(this->riff), 1, this->ptr);
    
    nread = fread(buffer4, sizeof(buffer4), 1, this->ptr);
    this->overall_size = (buffer4[0]) | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
    
    nread = fread(this->wave, sizeof(this->wave), 1, this->ptr);
    
    nread = fread(this->format, sizeof(this->format), 1, this->ptr);
    
    nread = fread(buffer4, sizeof(buffer4), 1, this->ptr);
    this->format_size = (buffer4[0]) | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
    
    nread = fread(buffer2, sizeof(buffer2), 1, this->ptr);
    
    this->format_type = (buffer2[0]) | (buffer2[1]<<8);
    
    nread = fread(buffer2, sizeof(buffer2), 1, this->ptr);
    this->channels = (buffer2[0]) | (buffer2[1]<<8);
    
    nread = fread(buffer4, sizeof(buffer4), 1, this->ptr);
    this->sample_rate = (buffer4[0]) | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
    
    nread = fread(buffer4, sizeof(buffer4), 1, this->ptr);
    this->byte_rate = (buffer4[0]) | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
    
    nread = fread(buffer2, sizeof(buffer2), 1, this->ptr);
    this->block_align = (buffer2[0]) | (buffer2[1]<<8);
    
    nread = fread(buffer2, sizeof(buffer2), 1, this->ptr);
    this->bits_per_sample = (buffer2[0]) | (buffer2[1]<<8);
    
    nread = fread(this->data, sizeof(this->data), 1, this->ptr);
    
    nread = fread(buffer4, sizeof(buffer4), 1, this->ptr);
    this->data_size = (buffer4[0]) | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
    
    this->num_samples = (this->data_size) / (8 * this->channels * this->bits_per_sample);
    this->samples = (kiss_fft_cpx*) malloc(sizeof(kiss_fft_cpx)*(this->num_samples));
    
    long size_of_sample = (this->channels * this->bits_per_sample) / 8; //in bytes
    
    if(this->format_type == 1) {
        char c = 'y';
        if(c == 'Y' || c == 'y') {
            char data_buffer[size_of_sample];
            int size_is_correct = TRUE;
            
            long bytes_in_each_channel = (size_of_sample / this->channels);
            if((bytes_in_each_channel * this->channels) != size_of_sample) {
                printf("Error, size is incorrect\n");
                size_is_correct = FALSE;
            }
            
            if(size_is_correct) {
                long low_limit = 0l;
                long high_limit = 0l;
                
                switch(this->bits_per_sample) {
                    case 8:
                        low_limit = -128;
                        high_limit = 127;
                        break;
                    case 16:
                        low_limit = -32768;
                        high_limit = 32767;
                        break;
                    case 32:
                        low_limit = -2147483648;
                        high_limit = 2147483647;
                        break;
                }
                
                for(unsigned i = 1; i <= this->num_samples; i++) {
                    nread = fread(data_buffer, sizeof(data_buffer), 1, this->ptr);
                    if(nread == 1) {
                        unsigned int xchannels = 0;
                        int data_in_channel = 0;
                        
                        for(xchannels = 0; xchannels < this->channels; xchannels++) {
                            if (bytes_in_each_channel == 4) {
                                data_in_channel = data_buffer[0] | (data_buffer[1]<<8) | (data_buffer[2]<<16) | (data_buffer[3]<<24);
                            }
                            else if(bytes_in_each_channel == 2) {
                                data_in_channel = data_buffer[0] | (data_buffer[1]<<8);
                            }
                            else if(bytes_in_each_channel == 1) {
                                data_in_channel = data_buffer[0];
                            }
                            
                            if(data_in_channel < low_limit || data_in_channel > high_limit) {
                            }
                            else {
                                this->samples[i].r += (kiss_fft_scalar)data_in_channel;
                            }
                        }
                        
                        this->samples[i].r = this->samples[i].r / this->channels;
                        this->samples[i].i = 0;
                    }
                    else {
                        printf("Error reading file. %d bytes\n", (int)nread);
                        break;
                    }
                }//for int i = 1<=num_samples;i++
            }//if size_is_correct
        }//if c == y
    }//if PCM
    this->time = (double)this->overall_size * 8/((double)this->sample_rate * (double)this->channels * (double)this->bits_per_sample);
    fclose(this->ptr);
    return 0;
}



FILE_INFO::~FILE_INFO()
{
    free(this->bins);
    free(this->fft_config);
    if(this->filename != NULL)
    {
        free(this->filename);
        free(this->samples);
    }
}
