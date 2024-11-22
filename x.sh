#!/bin/sh

cleanup() {
    echo -e "\n🛑  Process interrupted. Exiting gracefully..."
    exit 0
}

trap cleanup SIGINT

validate_email() {
    [[ "$1" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$ ]]
}

validate_phone() {
    [[ "$1" =~ ^[+]+[0-9]{8,15}$ ]]
}


requires_sudo() {
    local path="$1"
    if [[ "$path" == /usr/* || "$path" == /etc/* || "$path" == /var/* ]]; then
        return 0
    else
        return 1
    fi
}


create_directory() {
    local path="$1"
    if requires_sudo "$path"; then
        echo "✋  Path $path requires root access."
        echo "🔐  Entering sudo-enabled environment..."
        sudo mkdir -p "$path"
    else
        mkdir -p "$path"
    fi
}


write_certificate() {
    local path="."
    local content="$1"
    local file="$path/certificate.txt"

    if [[ -f ./LICENSE ]]; then
        license_content=$(cat ./LICENSE)
        content="$content\n\n--- LICENSE ---\n$license_content"
    else
        echo "⚠️  LICENSE file not found. Proceeding without it."
    fi

        echo -e "$content" > "$file"
}

write_config() {
    local config_file="install_config.ini"

    echo "[user]" > "$config_file"
    echo "full_name=$full_name" >> "$config_file"
    echo "email=$email" >> "$config_file"
    echo "phone=$phone" >> "$config_file"
    
    echo >> "$config_file"
    echo "[paths]" >> "$config_file"
    echo "cert_path=$cert_path" >> "$config_file"
    echo "temp_path=$temp_path" >> "$config_file"
    echo "save_path=$save_path" >> "$config_file"
    echo "docs_path=$docs_path" >> "$config_file"

    echo >> "$config_file"
    echo "[license]" >> "$config_file"
    echo "license_type=MIT License" >> "$config_file"
    echo "install_date=$install_date" >> "$config_file"
}


echo -e "🌟 Welcome to the Fuse setup script! Let's get everything ready for you!\n"
echo "📝  Please enter your details."

read -p "👤  Enter your full name: " full_name

while true; do
    read -p "📧  Enter your E-mail: " email
    if validate_email "$email"; then
        break
    else
        echo "❌  Invalid E-mail. Please try again."
    fi
done

while true; do
    read -p "📱  Enter your phone number +(8-15 numbers): " phone
    if validate_phone "$phone"; then
        break
    else
        echo "❌  Invalid phone number. Please try again."
    fi
done


echo "🔧  Please specify the installation paths."

read -p "📂  Enter path for the certificate installation [/usr/etc/$USER/fuse]: " cert_path
cert_path=${cert_path:-/usr/etc/$USER/fuse}

read -p "🗃️  Enter path for temporary files [/tmp/$USER/fuse]: " temp_path
temp_path=${temp_path:-/tmp/$USER/fuse}

read -p "💾  Enter path for save files [~/$USER/fuse/saves]: " save_path
save_path=${save_path:-~/fuse/saves}

read -p "📚  Enter path for documentation [~/$USER/fuse/docs]: " docs_path
docs_path=${docs_path:-~/fuse/docs}


install_date=$(date +"%Y-%m-%d")
certificate="Certificate of Free Use\n
User: $full_name\n
E-mail: $email\n
Phone: $phone\n
License: MIT License\n
Install Date: $install_date\n"

create_directory "$cert_path"
create_directory "$temp_path"
create_directory "$save_path"
create_directory "$docs_path"

write_certificate "$certificate"

write_config "install.config"

echo -e "\nSetup completed:"
echo "✅  Certificate will be saved in $cert_path/certificate.txt"
echo "📂  Temporary files will be stored in $temp_path"
echo "💾  Save files will be stored in $save_path"
echo "📚  Documentation will be stored in $docs_path"
echo -e "\n⚡️  Please now run \"make && sudo make install\" to install Fuse"