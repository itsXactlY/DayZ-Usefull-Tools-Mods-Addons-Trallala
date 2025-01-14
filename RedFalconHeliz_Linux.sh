#!/bin/bash
# ./start_workshopupdate.sh # - Start DayZ server with configurations and mods

# ========================
# Configurations
# ========================
readonly SERVER_BINARY="./DayZServer"
readonly CONFIG_FILE="serverDZ.cfg"
readonly PROFILES_FOLDER="./profile"
readonly LOG_FILE="server_startup.log"
readonly CPU_COUNT=8
readonly MAX_MEMORY=16132
readonly MODS_FOLDER="../DayZServer_Mods"

# Mod list (easily extendable)
readonly MOD_LIST=(
  "@RedFalcon"
)

readonly SERVER_MODS=(
  "@RedFalcon"
)

# ========================
# Helper Functions
# ========================

# Log messages to both console and log file
log() { echo "$(date '+%Y-%m-%d %H:%M:%S') - $*" | tee -a "$LOG_FILE"; }

# Exit with an error message and code
fail() { log "Error: $1"; exit "${2:-1}"; }

# Ensure required files exist
ensure_exists() { [[ -e "$1" ]] || fail "$2 not found: $1" "$3"; }

# Join mods into a single string with a semicolon delimiter, prefixed by the MODS_FOLDER path
join_mods() {
  local mods=()
  for mod in "${MOD_LIST[@]}"; do
    mods+=("$MODS_FOLDER/$mod")
  done
  local IFS=";" # Use semicolon as the delimiter
  echo "${mods[*]}" # Join elements
}

join_server_mods() {
  local server_mods=()
  for mod in "${SERVER_MODS[@]}"; do
    server_mods+=("$MODS_FOLDER/$mod")
  done
  local IFS=";"
  echo "${server_mods[*]}"
}

# Build DayZ server options
build_options() {
  local mods
  mods=$(join_mods)
  local server_mods
  server_mods=$(join_server_mods)
  echo "-config=$CONFIG_FILE -profiles=$PROFILES_FOLDER \"-serverMod=$server_mods\" \"-mod=$mods\" -nosound -noPause -cpuCount=$CPU_COUNT -maxMem=$MAX_MEMORY -dologs -logs -adminlog -scriptDebug" # -dologs -logs -adminlog
}

# Delete log files in profiles folder
delete_log_files() {
  log "Deleting log files in profiles folder..."
  find "$PROFILES_FOLDER" -type f \( -name "*.log" -o -name "*.RPT" -o -name "*.ADM" -o -name "*.mdmp" \) -delete
  log "Log files deleted successfully."
}

# ========================
# Main Logic
# ========================

# Validate required files
ensure_exists "$SERVER_BINARY" "Server binary" 101
ensure_exists "$CONFIG_FILE" "Configuration file" 102

# Validate mods folder
ensure_exists "$MODS_FOLDER" "Mods folder" 103

# Delete log files in profiles folder
delete_log_files

# Build server options
OPTIONS=$(build_options)

# Log startup
log "Starting server with options: $OPTIONS"

# Start the server
konsole --noclose -e fish -c "\"$SERVER_BINARY\" $OPTIONS" || fail "Server failed to start"

# Log success
log "Server started successfully."
