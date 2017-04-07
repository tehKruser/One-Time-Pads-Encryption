# One-Time-Pads-Encryption
Five small programs that encrypt and decrypt information using a one-time pad-like system.

Combines multi-processing code with socket-based inter-process communication. The program is accessible from the command line using standard UNIX features like input/output redirection, and job control. Compiles from a short compilation script.

***************************************************************************************
Example

Here is an example of usage, if you were testing your code from the command line:

$ cat plaintext1

THE RED GOOSE FLIES AT MIDNIGHT

$ otp_enc_d 57171 &

$ otp_dec_d 57172 &

$ keygen 10 > myshortkey

$ otp_enc plaintext1 myshortkey 57171 > ciphertext1 

Error: key ‘myshortkey’ is too short

$ echo $?

1

$ keygen 1024 > mykey

$ otp_enc plaintext1 mykey 57171 > ciphertext1

$ cat ciphertext1

GU WIRGEWOMGRIFOENBYIWUG T WOFL

$ keygen 1024 > mykey2

$ otp_dec ciphertext1 mykey 57172 > plaintext1_a

$ otp_dec ciphertext1 mykey2 57172 > plaintext1_b

$ cat plaintext1_a

THE RED GOOSE FLIES AT MIDNIGHT

$ cat plaintext1_b

WVIOWBTUEIOBC  FVTROIROUXA JBWE

$ cmp plaintext1 plaintext1_a

$ echo $?

0

$ cmp plaintext1 plaintext1_b

plaintext1 plaintext1_b differ: byte 1, line 1

$ echo $?

1

$ otp_enc plaintext5 mykey 57171

otp_enc_d error: input contains bad characters

$ otp_enc plaintext3 mykey 57172

Error: could not contact otp_enc_d on port 57172

$ echo $?

2

$

