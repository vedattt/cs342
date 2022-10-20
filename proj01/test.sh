#!/bin/bash

echo "pword with N = { 1, 2, 3, 4, 5 }"
./pword 1024 testout.txt 1 testin.txt | tail -1
./pword 1024 testout.txt 2 testin.txt testin.txt | tail -1
./pword 1024 testout.txt 3 testin.txt testin.txt testin.txt | tail -1
./pword 1024 testout.txt 4 testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 1024 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
echo

echo "pword with N = 5, msgsize = { 128, 256, 512, 1024, 2048, 4096 }"
./pword 128 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 256 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 512 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 1024 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 2048 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
./pword 4096 testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
echo

echo "tword with N = { 1, 2, 3, 4, 5 }"
./tword testout.txt 1 testin.txt | tail -1
./tword testout.txt 2 testin.txt testin.txt | tail -1
./tword testout.txt 3 testin.txt testin.txt testin.txt | tail -1
./tword testout.txt 4 testin.txt testin.txt testin.txt testin.txt | tail -1
./tword testout.txt 5 testin.txt testin.txt testin.txt testin.txt testin.txt | tail -1
echo

