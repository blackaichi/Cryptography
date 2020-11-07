#!/bin/sh

cp $1 res.pem

echo "-----BEGIN AES-128-CBC IV-----" >> res.pem
cat iv.bin | openssl enc -a >> res.pem
echo "-----END AES-128-CBC IV-----" >> res.pem

echo "-----BEGIN AES-128-CBC CIPHERTEXT-----" >> res.pem
cat ciphertext.bin | openssl enc -a >> res.pem
echo "-----END AES-128-CBC CIPHERTEXT-----" >> res.pem

echo "-----BEGIN SHA256-HMAC TAG-----" >> res.pem
cat tag.bin | openssl enc -a >> res.pem
echo "-----END SHA256-HMAC TAG-----" >> res.pem