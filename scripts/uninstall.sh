
BIN_PATH=$(awk -F '=' '/bin_path/{print $$2}' /etc/fuse/config.ini)
CERT_PATH=$(awk -F '=' '/cert_path/{print $$2}' /etc/fuse/config.ini)
SAVE_PATH=$(awk -F '=' '/save_path/{print $$2}' /etc/fuse/config.ini)
DOCS_PATH=$(awk -F '=' '/docs_path/{print $$2}' /etc/fuse/config.ini)

echo -e "🧹 \033[33m Cleaning fuse binary \033[0m"
sudo rm -rf /usr/bin/fuse
echo -e "🧹 \033[33m Cleaning certificate \033[0m"
sudo rm -rf $CERT_PATH/certificate.txt
echo -e "🧹 \033[33m Cleaning fuse binary \033[0m"
sudo rm -rf $BIN_PATH
echo -e "🧹 \033[33m Cleaning docs \033[0m"
sudo rm -rf $DOCS_PATH
echo -e "🧹 \033[33m Cleaning temp files directory \033[0m"
sudo rm -rf $TEMP_PATH

echo -e "🧹 \033[33m Cleaning/etc/fuse/  \033[0m"




sudo rm -rf /etc/fuse/
echo -e "🧹 \033[33m Fuse uninstalled from your computer \033[0m"
