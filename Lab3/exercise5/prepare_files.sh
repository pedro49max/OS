#!/bin/bash


#check the number of arguments
if [ $# -ne 1 ];then
    echo "Usage: $0 <directory>"
    exit 1
fi

DIR=$1

#cretate and acces the directory
if [ -d "$DIR" ]; then
    #if exits remove contents
    echo "Directory $DIR exits. Deleting its contents..."
    rm -rf "$DIR"/*
else
    #if does not exists create it
    echo "Directory $DIR does not exist. Creating it..."
    mkdir $DIR
fi

#acces directory
cd "$DIR" || exit 1

#cretaing the files
echo "creating files and links in $DIR..."

mkdir subdir
touch file1
echo "asdfghjklñ" > file2

#simbolic link to file2
ln -s file2 Slink
#hard link to file2
ln file2 Hlink

#Displaying atributes
echo "displaying file atributes"
for file in subdir file1 file2 Slink Hlink; do
    echo"Atributes of $file:"
    stat "$file"
    echo "--------------------------------"
done

exit 0;

#chmod +x prepare_files.sh
#./prepare_files.sh testdir

#question1: file1 0 blocks, and file2 8 blocks

#question2: subdir 4096 bytes

#question3: ls -ia       file2 and hLink share the i-node nad Slink has a similar i-node. The subdirectories have a '.'

#question4: no, with the Slink it says that it does not exists in the directory

#question5: touch -a Hlink for access   touch -m Hlink for modification