#!/bin/sh

openssl genpkey -genparam -algorithm dh -pkeyopt dh_rfc5114:3 -out param.pem

openssl genpkey -paramfile param.pem -out my_pkey.pem

openssl pkey -in my_pkey.pem -pubout -out my_pubkey.pem

openssl genpkey -paramfile param.pem -text -out myephpkey.pem

openssl pkey -in myephpkey.pem -pubout -out myephpubkey.pem
