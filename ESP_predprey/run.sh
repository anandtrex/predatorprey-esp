#!/bin/bash
rm -f *.o
make
esp-predprey config_file.txt 
