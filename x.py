#!/bin/python

import os
import re
import sys
import signal
from datetime import datetime


def cleanup(signal_received, frame):
    print("\n🛑  Process interrupted. Exiting gracefully...")
    sys.exit(0)


signal.signal(signal.SIGINT, cleanup)


def validate_email(email):
    return re.match(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$", email)


def validate_phone(phone):
    return re.match(r"^[+]+[0-9]{8,15}$", phone)


def requires_sudo(path):
    return path.startswith(("/usr/", "/etc/", "/var/"))


def create_directory(path):
    if requires_sudo(path):
        print(f"✋  Path {path} requires root access.")
        print("🔐  Entering sudo-enabled environment...")
        os.system(f"sudo mkdir -p {path}")
    else:
        os.makedirs(path, exist_ok=True)


def write_certificate(content, path="."):
    file_path = os.path.join(path, "certificate.txt")
    license_path = "./LICENSE"

    if os.path.isfile(license_path):
        with open(license_path, "r") as f:
            license_content = f.read()
        content += f"\n\n--- LICENSE ---\n{license_content}"
    else:
        print("⚠️  LICENSE file not found. Proceeding without it.")

    with open(file_path, "w") as f:
        f.write(content)


def write_config(config_file, user_data, paths, license_data):
    with open(config_file, "w") as f:
        f.write("[user]\n")
        f.write(f"full_name={user_data['full_name']}\n")
        f.write(f"email={user_data['email']}\n")
        f.write(f"phone={user_data['phone']}\n\n")
        
        f.write("[paths]\n")
        for key, value in paths.items():
            f.write(f"{key}={value}\n")
        f.write("\n")
        
        f.write("[license]\n")
        for key, value in license_data.items():
            f.write(f"{key}={value}\n")


def main():
    print("🌟 Welcome to the Fuse setup script! Let's get everything ready for you!\n")
    print("📝  Please enter your details.")
    
    full_name = input("👤  Enter your full name: ").strip()

    while True:
        email = input("📧  Enter your E-mail: ").strip()
        if validate_email(email):
            break
        print("❌  Invalid E-mail. Please try again.")
    
    while True:
        phone = input("📱  Enter your phone number +(8-15 numbers): ").strip()
        if validate_phone(phone):
            break
        print("❌  Invalid phone number. Please try again.")

    print("🔧  Please specify the installation paths.")

    cert_path = input("📂  Enter path for the certificate installation [/usr/etc/$USER/fuse]: ").strip() or f"/usr/etc/{os.getenv('USER')}/fuse"
    temp_path = input("🗃️  Enter path for temporary files [/tmp/$USER/fuse]: ").strip() or f"/tmp/{os.getenv('USER')}/fuse"
    save_path = input("💾  Enter path for save files [~/$USER/fuse/saves]: ").strip() or f"{os.path.expanduser('~')}/fuse/saves"
    docs_path = input("📚  Enter path for documentation [~/$USER/fuse/docs]: ").strip() or f"{os.path.expanduser('~')}/fuse/docs"

    install_date = datetime.now().strftime("%Y-%m-%d")
    certificate = (
        f"Certificate of Free Use\n\n"
        f"User: {full_name}\n\n"
        f"E-mail: {email}\n\n"
        f"Phone: {phone}\n\n"
        f"License: MIT License\n\n"
        f"Install Date: {install_date}\n"
    )

    create_directory(cert_path)
    create_directory(temp_path)
    create_directory(save_path)
    create_directory(docs_path)

    write_certificate(certificate)

    config_file = "install_config.ini"
    user_data = {"full_name": full_name, "email": email, "phone": phone}
    paths = {"cert_path": cert_path, "temp_path": temp_path, "save_path": save_path, "docs_path": docs_path}
    license_data = {"license_type": "MIT License", "install_date": install_date}

    write_config(config_file, user_data, paths, license_data)

    print("\nSetup completed:")
    print(f"✅  Certificate will be saved in {cert_path}/certificate.txt")
    print(f"📂  Temporary files will be stored in {temp_path}")
    print(f"💾  Save files will be stored in {save_path}")
    print(f"📚  Documentation will be stored in {docs_path}")
    print("\n⚡️  Please now run 'make && sudo make install' to install Fuse")


if __name__ == "__main__":
    main()
