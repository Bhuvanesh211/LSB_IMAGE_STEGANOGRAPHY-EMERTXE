# LSB_IMAGE_STEGANOGRAPHY-EMERTXE-
C PROJECT(Using Ubuntu(Linux) platform)
Compile all the C files using:

    gcc  *.c

For encoding, you have to use -e followed by an input image filename followed by a secret filename.
(Note that these files must be present in the current project directory, otherwise an error message appears. Also note that the input image filename must be having ".bmp" format to avoid error while the secret file can be of any format: .c, .cpp, .txt, .vhdl etc. But the format type cannot exceed more than 4 characters because a macro MAX_FILE_SUFFIX has been set to 4. Also don't miss the dot in any filename. It will result in an error). Example -

    ./a.out -e beautiful.bmp secret.txt

For decoding, you have to use -d followed by output image filename. Example -

    ./a.out -d stego.bmp

It will decode the secret message from the image and store it in a newly created file called "decoded.txt" which is default decoded filename. This file has the same format as that of the input secret file format.
If you want to provide your own decoded filename, you can do so by using 3rd argument as shown below:

    ./a.out -d stego.bmp output.txt

(Note that the file format must match with that of the input secret file's format. Otherwise an error message appears)
    

