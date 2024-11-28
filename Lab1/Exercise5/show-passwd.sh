#!/bin/bash

# Read /etc/passwd line by line
while IFS=':' read -r login_name encrypted_passwd uid gid user_name home_dir shell; do
    # Check if the home directory starts with /home
    if [[ $home_dir == /home* ]]; then
        printf "[Entry]\n"
        printf "\tlogin=%s\n" "$login_name"
        printf "\tenc_pass=%s\n" "$encrypted_passwd"
        printf "\tuid=%d\n" "$uid"
        printf "\tgid=%d\n" "$gid"
        printf "\tuser_name=%s\n" "$user_name"
        printf "\thome=%s\n" "$home_dir"
        printf "\tshell=%s\n" "$shell"
        printf "\n"
    fi
done < ~/etc/passwd
