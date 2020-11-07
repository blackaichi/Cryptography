## Steps to encrypt:

1. Generate a public and private key pairs and the ephimeral private and public keys.
```console
./generation.sh
```

2. Encrypt the message.
```console
./encryption pub_key_receiver message.txt
```

3. Build the pem file that we will send to the other part.
```console
./parserBuild my_pub_key.pem
```
Usually "my_pub_key.pem" will be "myephpubkey.pem"

4. Send to the receiver.


## Steps to decrypt:

1. Use the parser to get the files sent by the sender. 
```console
./parserSplit.sh name_file.pem
```

2. Decrypt the file.
```console
./decryption.sh my_priv_key.pem
```
Usually "my_priv_key.pem" will be "myephpkey.pem"