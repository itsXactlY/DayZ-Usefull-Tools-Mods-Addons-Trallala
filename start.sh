#!/bin/bash
./start_workshopupdate.sh # - Start DayZ server with configurations and mods

# ========================
# Configurations
# ========================
readonly SERVER_BINARY="./DayZServer"
readonly CONFIG_FILE="serverDZ.cfg"
readonly PROFILES_FOLDER="./profile"
readonly LOG_FILE="server_startup.log"
readonly CPU_COUNT=8
readonly MAX_MEMORY=16132

# Mod list (easily extendable)
#
#  "@Paragon Gear and Armor"
#  "@SNAFU_Weapons"
#  "@RevGuns"
#  "@Traveling Trader"
readonly MOD_LIST=(
  "@CF"
  "@Dabs Framework"
  "@BaseBuildingPlus"
  "@BodyBags"
  "@Breachingcharge"
  "@CJ187-LootChest"
  "@Code Lock"
  "@MuchFramework"
  "@MuchStuffPack"
  "@VPPAdminTools"
  "@Trader"
  "@TruckFixV2"
  "@Wilmas BBP Item Drop Fix"
  "@CJ187-Cars"
  "@MuchCarKey"
  "@Forward Operator Gear"
  "@AmmunitionExpansion"
  "@EDO_WEAPONS"
  "@Ear Plugs"
  "@Inventory Move Sounds"
  "@MaharlikaPH_Boats"
  "@Survivor Animations"
  "@CarCover"
  "@Care Packages V2"
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

# Join mods into a single string with a semicolon delimiter, enclosed in double quotes
join_mods() {
  local IFS=";" # Use semicolon as the delimiter
  echo "\"$*\"" # Join elements and wrap the result in quotes
}

# Build DayZ server options
build_options() {
  local mods=$(join_mods "${MOD_LIST[@]}") # Properly formatted mod list
  echo "-config=$CONFIG_FILE -profiles=$PROFILES_FOLDER -mod=$mods -nosound -noPause -cpuCount=$CPU_COUNT -maxMem=$MAX_MEMORY -dologs -logs -adminlog -scriptDebug -persist"
}

# ========================
# Main Logic
# ========================

# Validate required files
ensure_exists "$SERVER_BINARY" "Server binary" 101
ensure_exists "$CONFIG_FILE" "Configuration file" 102

# Build server options
OPTIONS=$(build_options)

# Log startup
log "Starting server with options: $OPTIONS"

# Start the server
konsole --noclose -e fish -c "\"$SERVER_BINARY\" $OPTIONS" || fail "Server failed to start"

# Log success
log "Server started successfully."
