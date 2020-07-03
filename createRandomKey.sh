#!/usr/bin/env bash

# Using openssl library to generate random byte string

# For use in RC4 Stream Cipher:
# -Must be between 5 and 256 bytes
# -64 and 128 bytes are the most common

openssl rand 128 > key.txt


