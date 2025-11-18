#pragma once

#include "domain/AbcSong.h"
#include "domain/Duration.h"
#include "services/AbcParser.h"
#include "infrastructure/FileAbcRepository.h"
#include <vector>
#include <string>
#include <memory>

namespace setlistgui {

struct TitleLine {
    std::string fullTitle;         // Complete T: line content
    std::string originalFullTitle; // Original full title for comparison
    std::string instrument;        // Extracted instrument name (for display)
    bool titleEdited;              // Track if full title was edited
};

struct SongCard {
    std::string filename;
    std::string originalFilePath;  // Full path to original file
    std::string originalContent;   // Original ABC file content
    std::string title;             // First title (for main display)
    std::string originalTitle;     // Original title for comparison
    std::vector<TitleLine> titleLines; // All T: lines with instruments
    int durationSeconds;
    std::vector<std::string> instruments; // All instruments (for display)
    int order;  // For reordering
    bool titleEdited;              // Track if main title was edited
};

class SetlistManager {
public:
    SetlistManager();

    // Add song from file path
    bool addSongFromFile(const std::string& filepath);

    // Remove song at index
    void removeSong(size_t index);

    // Reorder songs (move from oldIndex to newIndex)
    void reorderSong(size_t oldIndex, size_t newIndex);

    // Update song title
    void updateSongTitle(size_t index, const std::string& newTitle);

    // Update full title line for specific part
    void updateTitleLine(size_t songIndex, size_t titleLineIndex, const std::string& newFullTitle);

    // Get all songs
    const std::vector<SongCard>& getSongs() const { return songs_; }

    // Calculate total duration with padding and intro
    showtimecalc::domain::Duration getTotalDuration(int paddingSeconds, int introSeconds) const;

    // Export setlist - copy files to folder with edits applied
    bool exportToFolder(const std::string& folderPath, bool addNumbering = true) const;

    // Clear all songs
    void clear() { songs_.clear(); }

private:
    std::vector<SongCard> songs_;
    std::shared_ptr<showtimecalc::services::AbcParser> parser_;
    std::shared_ptr<showtimecalc::infrastructure::FileAbcRepository> repository_;

    // Extract instruments from ABC file content
    std::vector<std::string> extractInstruments(const std::string& content);

    // Extract all title lines with instruments
    std::vector<TitleLine> extractTitleLines(const std::string& content);
};

} // namespace setlistgui
