# ABC Setlist Manager

A Windows GUI application for managing musical performance setlists from ABC notation files. Built with Dear ImGui, GLFW, and C++17.

## Features

- **Drag-and-drop** .abc files to add songs to your setlist
- **Song cards** display:
  - Song title (double-click to edit - shows * indicator when edited)
  - Duration in M:SS format
  - List of instruments (extracted from ABC file)
  - "Edit Parts" button for multi-part songs (edit individual title lines)
  - Filename reference
- **Reorder songs** by dragging cards up/down
- **Remove songs** with one click
- **Live total duration** calculation with configurable:
  - Padding between songs (in seconds)
  - Intro time (in seconds)
- **Export to folder** - copies all ABC files to a destination folder:
  - Optional numbering: toggle on/off to add order prefix (01_, 02_, etc.)
  - Any title edits are automatically saved to the new files
  - Individual title line edits (for multi-part songs) are applied to exported files
  - Original files remain unchanged

## Screenshots

The application features:
- Clean, dark-themed interface with ImGui
- Large, prominent total duration display
- Scrollable song list with colorful song cards
- Input controls for padding and intro time
- Native Windows folder browser dialog
- Export controls with numbering toggle
- Real-time feedback messages

## Building the Project

### Prerequisites

- **CMake** 3.14 or higher
- **C++17 compatible compiler**:
  - Windows: MSVC 2017+ or MinGW-w64
  - Linux: GCC 7+ or Clang 5+
- **OpenGL** support (usually comes with graphics drivers)
- **Internet connection** (for downloading dependencies during build)

### Dependencies (Auto-downloaded)

CMake will automatically fetch these dependencies:
- **Dear ImGui** v1.90.1 (GUI framework)
- **GLFW** 3.3.8 (windowing and OpenGL context)

The project also reuses code from the `show-time-calculator` example:
- AbcParser, Duration, AbcSong domain classes
- FileAbcRepository infrastructure

### Build Instructions

**Step 1: Setup Dependencies**

First, download and extract the required libraries:

```bash
cd abc-setlist-gui
setup-dependencies.bat
```

This will download GLFW and Dear ImGui to the `deps/` folder.

**Step 2: Build the Application**

**Windows (MinGW - Recommended):**

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

The executable will be at: `build/abc-setlist-gui.exe`

**Windows (MSVC):**

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable will be at: `build/Release/abc-setlist-gui.exe`

**Linux:**

```bash
cd abc-setlist-gui
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be at: `build/abc-setlist-gui`

**Note for Linux:** You may need to install OpenGL development packages:
```bash
# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev

# Fedora
sudo dnf install mesa-libGL-devel mesa-libGLU-devel
```

## Usage

### Running the Application

**Windows:**
```bash
./build/Release/abc-setlist-gui.exe
```

**Linux:**
```bash
./build/abc-setlist-gui
```

### Using the Application

1. **Add Songs:**
   - Drag .abc files from your file explorer onto the application window
   - Songs will appear as cards in the list
   - Each card shows title, duration, and instruments

2. **Edit Song Titles:**
   - Double-click on any song title to edit it
   - Press Enter to save or click elsewhere to cancel
   - Edited titles show an orange * indicator

3. **Edit Title Lines (Multi-part Songs):**
   - Songs with multiple T: lines show an "Edit Parts" button
   - Click "Edit Parts" to open a modal dialog
   - Double-click any title line to edit the ENTIRE line content
   - Edit the full title: song name, instrument in brackets, duration, etc.
   - Press Enter to save or click elsewhere to cancel
   - Edited lines show an orange * indicator
   - Instrument names are automatically re-extracted from brackets

4. **Reorder Songs:**
   - Click and hold on a song card
   - Drag it to a new position
   - Release to drop it in the new location

5. **Remove Songs:**
   - Click the "Remove" button on any song card

6. **Adjust Timing:**
   - Enter padding seconds (time between songs)
   - Enter intro seconds (time at the beginning)
   - Total duration updates automatically

7. **Export Setlist:**
   - Enter a folder path manually OR click "Browse..." to select a folder
   - Check/uncheck "Add numbering" to enable/disable file numbering
   - Click "Export to Folder" button
   - All ABC files are copied to the destination folder
   - If numbering enabled: files named `01_filename.abc`, `02_filename.abc`, etc.
   - If numbering disabled: files keep original names
   - Any edited titles are saved to the new files (main title and individual title lines)
   - Original files remain unchanged
   - Success/error message appears below the button

8. **Clear All:**
   - Click "Clear All" to remove all songs and start fresh

### Example Workflow

```bash
# Run the application
D:\Development\band-helper\abc-setlist-gui\build\abc-setlist-gui.exe

# OR from WSL location:
\\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui\build\abc-setlist-gui.exe
```

**Then in the application:**

1. **Add Songs**: Drag .abc files from your collection

2. **Adjust Settings**:
   - Set padding to 5 seconds
   - Set intro to 10 seconds
   - Total duration updates automatically

3. **Customize**:
   - Double-click song titles to edit (shows * indicator)
   - Click "Edit Parts" on multi-part songs to edit individual title lines
   - Drag cards up/down to reorder
   - Remove unwanted songs with red button

4. **Export**:
   - Click "Browse..." and select destination folder
   - Toggle "Add numbering" checkbox as desired
   - Click "Export to Folder"
   - Success message confirms export!

## ABC File Format Requirements

The application extracts information from ABC notation files:

### Duration Extraction

Duration must be in the title line in format `(M:SS)` or `(MM:SS)`:

```abc
X:1
T:Red Right Hand [Drums] (6:05)
```

### Instrument Extraction

Instruments are extracted from:

1. **Title brackets/parentheses:**
   ```abc
   T:Song Name [Drums] (4:22)
   T:Song Name (Guitar) (3:45)
   ```

2. **Part-name directives:**
   ```abc
   %%part-name Drums
   %%part-name Lead Guitar
   ```

### Multi-part Files

Files with multiple `X:` sections (different instrument parts) are supported. The application:
- Extracts all instrument names
- Uses the longest duration found across all parts

Example:
```abc
X:1
T:Song - Drums (3:27)
%%part-name Drums

X:2
T:Song - Guitar (3:30)
%%part-name Guitar

X:3
T:Song - Bass (3:46)    ← Uses this duration (longest)
%%part-name Bass
```

## Export Format

When you export to a folder, the application:

1. **Creates the destination folder** if it doesn't exist
2. **Optionally numbers files**: Toggle "Add numbering" checkbox to prefix files with `01_`, `02_`, etc.
3. **Applies title edits**: If you edited a song title, the exported file will have the updated title in its `T:` line
4. **Preserves originals**: Original source files are never modified

**Example with numbering enabled:**

Before export:
- Songs added from various folders
- "Red Right Hand" main title edited to "Red Right Hand (Opening)"
- Multi-part song has individual title lines edited (e.g., "Part 1 [Guitar] (4:22)" changed to "Part 1 [Lead Guitar] (4:22)")
- "Connected" kept original title

After exporting to `C:\MySetlist\` with numbering ON:
```
C:\MySetlist\
  ├── 01_11_RedRightHand_V605.abc     (T: line updated with new title)
  ├── 02_MultiPartSong.abc            (all edited T: lines updated in file)
  ├── 03_08_Connected_V355.abc        (exact copy of original)
  └── 04_11_ChasingCarsLive_V631.abc  (exact copy of original)
```

**Example with numbering disabled:**

After exporting to `C:\MySetlist\` with numbering OFF:
```
C:\MySetlist\
  ├── 11_RedRightHand_V605.abc     (T: line updated, original name kept)
  ├── 08_Connected_V355.abc        (exact copy of original)
  └── 11_ChasingCarsLive_V631.abc  (exact copy of original)
```

## Architecture

The application follows clean architecture principles:

### Layer Structure

```
UI Layer (ImGui)
    ↓
Application Layer (SetlistManager)
    ↓
Service Layer (AbcParser) ← Reused from show-time-calculator
    ↓
Domain Layer (AbcSong, Duration) ← Reused from show-time-calculator
    ↓
Infrastructure Layer (FileAbcRepository) ← Reused from show-time-calculator
```

### Key Components

- **SetlistManager** (`src/SetlistManager.cpp`): Core business logic
  - Manages song collection
  - Calculates total duration
  - Handles add/remove/reorder operations
  - Exports to JSON

- **main.cpp** (`src/main.cpp`): ImGui application
  - GLFW window setup
  - OpenGL rendering
  - Drag-and-drop callbacks
  - UI rendering loop

- **SongCard struct**: Lightweight display model containing:
  - Title, filename, duration, instruments, order

## Troubleshooting

### Build Issues

**Error: Dependencies not found**
- Run `setup-dependencies.bat` first before building
- Make sure GLFW and ImGui downloaded to `deps/` folder
- Check internet connection for downloads

**Error: C++17 not supported**
- Ensure your compiler supports C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- Update MinGW if using older version

**Error: CMakeCache.txt directory mismatch**
- Delete `build/` folder completely: `rmdir /s /q build`
- Create fresh build directory and run cmake again

**Error: Cannot find OpenGL**
- Windows: Update graphics drivers
- Linux: Install OpenGL development packages (see build instructions)

**Error: CMake version too old**
- Update CMake to 3.14 or higher

**Error: Filesystem not found**
- Ensure you're using C++17 compatible compiler
- GCC < 9.0 should automatically link `stdc++fs`

### Runtime Issues

**Window doesn't open**
- Check that graphics drivers support OpenGL 3.0+
- Try running from command line to see error messages
- Update graphics drivers if needed

**Files don't drop**
- Ensure files have `.abc` extension
- Try running as administrator (Windows) if permissions issue
- Drag files one at a time or in small batches

**Duration shows 0:00**
- Verify ABC file has duration in title: `T:Song Name (M:SS)`
- Check that duration format matches `(M:SS)` with parentheses
- Multi-part files should have duration in at least one T: line

**No instruments shown**
- Application shows "Unknown" if no instruments detected
- Add instrument names in title brackets: `T:Song [Drums] (4:22)`
- Or use `%%part-name` directive in ABC file

**Export fails**
- Verify folder path is valid and accessible
- Ensure you have write permissions to destination
- Try using "Browse..." button instead of typing path
- Check that destination drive has enough free space

**Browse button doesn't work**
- Windows-only feature - won't work on Linux builds
- Make sure you're running the Windows .exe version
- Try typing path manually if dialog doesn't appear

## License

This project is for educational and personal use. Demonstrates:
- Dear ImGui immediate-mode GUI programming
- Clean architecture in C++
- Dependency injection patterns
- File I/O and parsing
- JSON export functionality

## Related Projects

- **show-time-calculator**: Console application for batch calculating show times
- Shares the same ABC parsing and domain logic (located in parent project)

## Current Features (Complete)

✅ Drag-and-drop ABC file loading
✅ Song card display with title, duration, instruments
✅ Title editing with change tracking (shows * indicator)
✅ Multi-part title line editing (edit each T: line individually)
✅ Drag-to-reorder songs
✅ Remove individual songs
✅ Live total duration calculation
✅ Configurable padding and intro time
✅ Native Windows folder browser
✅ Optional file numbering toggle
✅ Export with all title edits applied (main titles and individual title lines)
✅ Success/error message feedback

## Possible Future Enhancements

Ideas for future versions:
- Save/load setlist configurations
- Print setlist to PDF
- Calculate stage time with breaks
- Theme customization (light mode)
- Undo/redo functionality
- Song search/filter within list
- Sort by title/duration
- Duplicate song detection
- Batch file operations
