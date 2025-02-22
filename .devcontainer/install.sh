#!/bin/bash

# Install GitHub CLI
echo "Installing GitHub CLI..."
curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg
sudo apt-key add /usr/share/keyrings/githubcli-archive-keyring.gpg
sudo apt-add-repository https://cli.github.com/packages
sudo apt update
sudo apt install gh -y

# Install Android SDK tools
echo "Installing Android SDK tools..."
sudo apt-get update && sudo apt-get install -y openjdk-11-jdk
wget -q https://dl.google.com/android/repository/commandlinetools-linux-6609375_latest.zip -O /tmp/cmdline-tools.zip
mkdir -p /usr/local/android-sdk/cmdline-tools
unzip -q /tmp/cmdline-tools.zip -d /usr/local/android-sdk/cmdline-tools
rm /tmp/cmdline-tools.zip
yes | /usr/local/android-sdk/cmdline-tools/cmdline-tools/bin/sdkmanager --licenses
/usr/local/android-sdk/cmdline-tools/cmdline-tools/bin/sdkmanager "platform-tools" "platforms;android-30"

# Update PATH
export ANDROID_SDK_ROOT=/usr/local/android-sdk
export PATH=$PATH:/usr/local/android-sdk/cmdline-tools/cmdline-tools/bin:/usr/local/android-sdk/platform-tools

echo "Installation complete."
