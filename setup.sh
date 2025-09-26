#!/bin/bash
# libmingw32_extended Project Setup Script
# Run this with: source setup.sh
# Use --latest-reset-pull to reset and pull latest changes

# SSH Agent auto-start for this project
eval "$(ssh-agent -s)" > /dev/null 2>&1
ssh-add ./.ssh/id_ed25519
echo "✓ SSH key loaded for libmingw32_extended project"

# Ensure .git exists, or init if missing
if [ ! -d ".git" ]; then
    echo "⚙️  No .git directory found, initializing repository..."
    git init
    git remote add origin git@github.com:dailker/libmingw32_extended.git
    git fetch origin main
    git checkout -b main origin/main
    echo "✓ Fresh repository initialized and checked out from origin/main"
else
    git remote add origin git@github.com:dailker/libmingw32_extended.git 2>/dev/null
    git branch --set-upstream-to=origin/main main > /dev/null 2>&1
fi

# Set git user configuration for this project
git config user.name "Ilker"
git config user.email "me@theilker.com"
echo "✓ Git user configured as Ilker <me@theilker.com>"

# Handle reset & pull latest
if [[ "$1" == "--latest-reset-pull" ]]; then
    echo "🔄 Resetting and pulling latest changes..."
    git fetch origin main
    git reset --hard origin/main
    git clean -fd
    echo "✓ Repository reset and updated to latest"
fi

echo "🚀 libmingw32_extended development environment ready!"
