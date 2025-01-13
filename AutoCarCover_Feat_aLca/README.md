# AutoCover for CarCover

An automatic car covering system for DayZ servers that works with CarCover mod.

## Prerequisites

1. Subscribe to CarCover mod on Steam Workshop:
- [CarCover on Steam Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=2303483532)
- Install the mod through DayZ Launcher/Steam

## Server Installation

### Step 1: Install PBO
Copy `AutoCoverForGoodCarCover.pbo` to:
Steam/steamapps/common/DayZServer/ServerMods/ # Or any folder u like to use.

### Step 2: Configure init.c
1. Navigate to:
Steam/steamapps/common/DayZServer/mpmissions/dayzOffline.chernarusplus/
2. Merge or replace the provided `init.c` with your existing one
3. Start Server with `-serverMod=AutoCoverForGoodCarCover`

## Features

- Automatic detection of uncovered vehicles within 150m of players
- Position tracking to prevent duplicate covering
- Player occupancy checks before covering
- Periodic checks every 60 seconds
- Server-side implementation

## Technical Details

The mod includes several configurable parameters in `init.c`:
- Check radius: 150m around players
- Check interval: 60 seconds

## Notes

- Backup your existing `init.c` before making changes
- Test the configuration on a development server before deploying to production
- Monitor server logs for any potential issues or conflicts

## Support

For issues or questions, please create an issue in the repository or contact the mod maintainer.

