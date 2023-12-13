#!/bin/bash

mkdir testData
cd testData

wget -O data.zip https://mattmahoney.net/dc/enwik9.zip
unzip data.zip

# Backup the file in case of errors or decompression overwrite
cp enwik9 enwik9.bk
