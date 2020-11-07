#!/bin/sh

openssl pkeyutl -inkey $1 -peerkey ephpubkey.pem -derive -out common2.bin

openssl dgst -sha256 -binary common2.bin | head -c 16 > k12.bin
openssl dgst -sha256 -binary common2.bin | tail -c 16 > k22.bin

cat iv.bin ciphertext.bin | openssl dgst -sha256 -mac hmac -binary -macopt hexkey:$(xxd -p k22.bin) -out tag2.bin

openssl enc -d -aes-128-cbc -K `xxd -p k12.bin` -iv `xxd -p iv.bin` -nopad -in ciphertext.bin -out messagedecrypted.txt
