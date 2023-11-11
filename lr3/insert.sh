#!/bin/bash

./packer data/kodim04.bmp data/payload.txt
rm data/result.bmp
mv result.bmp data/result.bmp