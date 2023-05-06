#!/bin/sh
g++  parent.cpp -pthread -lrt -o parent
g++ line.cpp -pthread -lrt -o line
g++ line_8.cpp -pthread -lrt -o line_8
g++ storage_area.cpp -pthread -lrt -o storageArea
g++ shipping_service.cpp -pthread -lrt -o shippingService
g++ drawer.cpp -o drawer -lglut -lGLU -lGL -lm -pthread
./parent