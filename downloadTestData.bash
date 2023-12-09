#!/bin/bash

mkdir testData
cd testData

wget -O data.zip https://mattmahoney.net/dc/enwik8.zip
unzip data.zip

# Backup the file in case of errors or decompression overwrite
cp enwik8 enwik8.bk
