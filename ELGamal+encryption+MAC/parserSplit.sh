#!/bin/sh

csplit -f individual- $1 '/-----BEGIN/' '{*}'

mv individual-01 ephpubkey.pem
sed -i '/-----/d' individual-02
sed -i '/-----/d' individual-03
sed -i '/-----/d' individual-04

cat individual-02 | openssl enc -d -a > iv.bin
cat individual-03 | openssl enc -d -a > ciphertext.bin
cat individual-04 | openssl enc -d -a > tag.bin

rm individual*
