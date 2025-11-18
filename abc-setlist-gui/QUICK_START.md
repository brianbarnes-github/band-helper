# Quick Start Guide

## Getting Started in 3 Steps

### Step 1: Build the Application (Windows)

**Part A - Setup Dependencies:**

1. Open Command Prompt or PowerShell
2. Navigate to the project:
   ```cmd
   cd D:\Development\band-helper\abc-setlist-gui
   ```
   OR if accessing from WSL:
   ```cmd
   cd \\wsl$\Ubuntu\home\brian\band-helper\abc-setlist-gui
   ```
3. Run the dependency setup script:
   ```cmd
   setup-dependencies.bat
   ```
   This downloads GLFW and ImGui to the `deps/` folder (only needed once).

**Part B - Build the Application:**

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

**Alternative - Use the all-in-one build script:**

```cmd
build-windows.bat
```

This automatically runs setup-dependencies if needed, then builds the project.

### Step 2: Run the Application

From the build directory:

```cmd
abc-setlist-gui.exe
```

Or simply double-click `abc-setlist-gui.exe` in File Explorer!

### Step 3: Add Your Songs

1. The application window opens with a dark theme
2. Drag and drop .abc files from your file explorer onto the window
3. Songs appear as cards showing:
   - Order number and title
   - Duration (M:SS format)
   - Instruments extracted from file
   - Remove button (red)
4. Adjust padding and intro seconds in the input boxes
5. Watch the **TOTAL DURATION** update in real-time

## Example: Using Your ABC Files

Try it with your own ABC notation files:

1. Run the application
2. Open File Explorer and navigate to a folder containing your .abc files
3. Select several .abc files (or all of them)
4. Drag them onto the ABC Setlist Manager window
5. They'll appear as cards in the list!

## Features at a Glance

- **Add Songs**: Drag .abc files onto window (multiple files at once supported)
- **Edit Title**: Double-click any song title (shows orange * when edited)
- **Edit Title Lines**: Click "Edit Parts" on multi-part songs to edit individual T: lines
- **Reorder**: Click and drag cards to new positions
- **Remove**: Click red "Remove" button on any card
- **Adjust Timing**: Change pad/intro seconds (recalculates automatically)
- **Export**:
  - Click "Browse..." to select destination folder
  - Toggle "Add numbering" checkbox (on = `01_`, `02_`, etc.)
  - Click "Export to Folder"
  - Success message confirms export with file count
- **Clear**: Click "Clear All" to start over

## Detailed Usage

### Editing Song Titles

1. Double-click any song title in a card
2. Text field appears - type your changes
3. Press Enter to save OR click elsewhere to cancel
4. Edited titles show an orange `*` indicator
5. Hover over `*` to see tooltip: "Title edited - changes will be saved on export"

### Editing Individual Title Lines (Multi-part Songs)

1. Songs with multiple T: lines show an "Edit Parts" button next to the instruments
2. Click "Edit Parts" to open a modal dialog
3. The modal shows each title line (e.g., "Part 1: Song Name [Guitar] (4:22)")
4. Double-click any title line to edit
5. Edit the ENTIRE title line - song name, instrument in brackets, duration, etc.
6. Press Enter to save OR click elsewhere to cancel
7. Edited title lines show an orange `*` indicator
8. Instrument names are automatically re-extracted from the edited title
9. Click "Done" to close the modal
10. On export, all edited title lines are saved to their respective T: lines in the file

### Reordering Songs

1. Click and hold on any song card
2. Drag up or down to desired position
3. Release mouse to drop in new position
4. Order numbers update automatically

### Exporting Your Setlist

1. **Select Destination**:
   - Click "Browse..." button
   - Navigate in native Windows folder picker
   - Select existing folder OR create new one
   - Selected path appears in text field
   - (You can also type path manually)

2. **Choose Numbering**:
   - Check "Add numbering" = files named `01_Song.abc`, `02_Song.abc`
   - Uncheck = files keep original names

3. **Export**:
   - Click "Export to Folder" button
   - Green success message appears: "Successfully exported X files to folder with numbering!"
   - OR red error message if something went wrong

4. **Check Results**:
   - Navigate to destination folder
   - Files are copied in order
   - Edited main titles are saved to the first T: line
   - Edited individual title lines are saved to their respective T: lines
   - Original files remain unchanged

## Troubleshooting

**Can't build?**
- Make sure you ran `setup-dependencies.bat` first
- Check that `deps/glfw-3.3.8` and `deps/imgui-1.90.1` folders exist
- Ensure you have Visual Studio with C++ tools OR MinGW-w64 installed
- See `BUILD_NOTES.md` for detailed instructions

**Songs not loading?**
- Verify .abc files have duration in title: `T:Song Name (M:SS)`
- Check file has `.abc` extension
- Try dragging files one at a time

**Duration shows 0:00?**
- ABC file must have time in parentheses in title line
- Format: `(M:SS)` or `(MM:SS)`
- Example: `T:My Song [Drums] (4:22)`

**Export fails?**
- Make sure folder path is valid and you have write permissions
- Use full paths like `C:\Users\YourName\Desktop\MySetlist`
- Folder will be created automatically if it doesn't exist
- Try using "Browse..." button instead of typing

**Browse button doesn't open dialog?**
- Windows-only feature
- Try typing path manually in text field
- Ensure you're running the Windows .exe (not Linux build)

**Edited title indicator (*) doesn't show?**
- Make sure you pressed Enter after editing
- Clicking away from the text field cancels the edit
- Title must be different from original to show indicator

**"Edit Parts" button doesn't appear?**
- Button only shows for songs with multiple T: lines (multi-part ABC files)
- Songs with a single T: line don't need the button
- You can still edit the main title by double-clicking it

## Keyboard Shortcuts

- **Enter**: Save edited song title
- **Escape**: Cancel editing (when text field is active)
- **Mouse drag**: Reorder songs

## Next Steps

- Read `README.md` for complete documentation
- Check `BUILD_NOTES.md` if you have build issues
- Try editing song titles and exporting to a folder
- For multi-part songs, use "Edit Parts" to edit individual title lines
- Drag to reorder songs in your preferred order
- Experiment with numbering on/off to see the difference
- Use the exported files for your performance!

## Tips

- **Planning a show?** Set your padding time to account for tuning between songs
- **Long setlist?** Use the intro time for sound check or opening
- **Multiple versions?** Export with different folder names for different show configurations
- **Backup originals:** Your source ABC files are never modified - only exported copies change
- **Batch operations:** Drag multiple files at once for faster setup

Enjoy managing your setlists! 🎵🎸
