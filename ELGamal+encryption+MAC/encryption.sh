#!/bin/sh

openssl pkeyutl -inkey myephpkey.pem -peerkey $1 -derive -out common.bin

openssl dgst -sha256 -binary common.bin | head -c 16 > k1.bin
openssl dgst -sha256 -binary common.bin | tail -c 16 > k2.bin

openssl rand 16 > iv.bin
openssl enc -aes-128-cbc -K `xxd -p k1.bin` -iv `xxd -p iv.bin` -in $2 -out ciphertext.bin

cat iv.bin ciphertext.bin | openssl dgst -sha256 -mac hmac -binary -macopt hexkey:$(xxd -p k2.bin) -out tag.bin
