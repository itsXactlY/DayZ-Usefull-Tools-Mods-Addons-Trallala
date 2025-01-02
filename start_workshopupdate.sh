#!/bin/bash

DAYZ_SERVER_PATH="/home/alca/.local/share/Steam/steamapps/common/DayZServer/"
STEAM_WORKSHOP_PATH="/home/alca/.local/share/Steam/steamapps/workshop/content/221100/"
STEAMCMD_PATH="/usr/bin/steamcmd"  # Or the path to your standalone SteamCMD

# Array to store mod IDs
MOD_IDS=()

# Loop through @modname directories
for MOD_PATH in "$DAYZ_SERVER_PATH"/@*; do
  # Get mod directory name (e.g., "@modname")
  MOD_NAME=$(basename "$MOD_PATH")

  # Check if the path is a directory
  if [[ ! -d "$MOD_PATH" ]]; then
    echo "Skipping non-directory: $MOD_PATH"
    continue
  fi

  # Initialize MOD_ID to empty
  MOD_ID=""

  # Check for meta.cpp and extract mod ID
  if [[ -f "$MOD_PATH/meta.cpp" ]]; then
    MOD_ID=$(grep -Po 'publishedid\s*=\s*"?\K\d+' "$MOD_PATH/meta.cpp" | head -n 1)
    if [[ -z "$MOD_ID" ]]; then
      echo "Warning: Could not extract mod ID from meta.cpp for $MOD_NAME. Possible formatting issue or missing ID."
    else
      echo "Found mod ID: $MOD_ID for $MOD_NAME"
    fi
  else
    echo "Warning: meta.cpp not found for $MOD_NAME."
  fi

  # If MOD_ID is still empty, try to find a matching directory in the workshop
  if [[ -z "$MOD_ID" ]]; then
    echo "Attempting to find matching workshop directory for $MOD_NAME..."
    POSSIBLE_MOD_ID=$(find "$STEAM_WORKSHOP_PATH" -maxdepth 1 -type d -name "*$MOD_NAME*" | sed -E 's,.*/([0-9]+),\1,' | head -n 1)

    if [[ -n "$POSSIBLE_MOD_ID" ]]; then
      echo "Possible matching workshop directory found: $POSSIBLE_MOD_ID"
      MOD_ID="$POSSIBLE_MOD_ID"
    else
      echo "Error: No matching workshop directory found for $MOD_NAME. Skipping update."
      continue
    fi
  fi

  # Add mod ID to the array
  MOD_IDS+=("$MOD_ID")
done

# Check if any mod IDs were found
if [[ ${#MOD_IDS[@]} -eq 0 ]]; then
  echo "No mod IDs found. Exiting."
  exit 0
fi

echo "Updating mods: ${MOD_IDS[*]}..."

# Construct SteamCMD command with multiple workshop_download_item commands
STEAMCMD_COMMAND=("$STEAMCMD_PATH" +@sSteamCmdForcePlatformType linux +login anonymous +force_install_dir "$DAYZ_SERVER_PATH")

for MOD_ID in "${MOD_IDS[@]}"; do
  STEAMCMD_COMMAND+=("+workshop_download_item 221100 $MOD_ID validate")
done

STEAMCMD_COMMAND+=("+quit")

# Execute SteamCMD command
"${STEAMCMD_COMMAND[@]}"

# Sync downloaded files with server directories
for MOD_ID in "${MOD_IDS[@]}"; do
  MOD_NAME=$(find "$DAYZ_SERVER_PATH" -name "@*" -type d -print0 | while IFS= read -r -d $'\0' mod_dir; do if grep -q "$MOD_ID" "$mod_dir/meta.cpp"; then basename "$mod_dir"; break; fi; done)
  if [[ -z "$MOD_NAME" ]]; then
    echo "Warning: Could not find server directory for mod ID $MOD_ID. Skipping rsync."
    continue
  fi
  rsync -av --delete "$STEAM_WORKSHOP_PATH/$MOD_ID/" "$DAYZ_SERVER_PATH/$MOD_NAME/"
done

echo "Mod update complete."