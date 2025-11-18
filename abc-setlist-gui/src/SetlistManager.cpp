#include "SetlistManager.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace setlistgui {

SetlistManager::SetlistManager() {
    parser_ = std::make_shared<showtimecalc::services::AbcParser>();
    repository_ = std::make_shared<showtimecalc::infrastructure::FileAbcRepository>();
}

bool SetlistManager::addSongFromFile(const std::string& filepath) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            return false;
        }

        // Read file content
        std::string content = repository_->readFile(filepath);

        // Get filename without path
        std::string filename = std::filesystem::path(filepath).filename().string();

        // Parse the ABC file
        auto abcSong = parser_->parse(filename, content);

        if (!abcSong || !abcSong->isValid()) {
            return false;
        }

        // Extract all title lines with instruments
        std::vector<TitleLine> titleLines = extractTitleLines(content);

        // Extract instruments for display
        std::vector<std::string> instruments = extractInstruments(content);

        // Create song card
        SongCard card;
        card.filename = filename;
        card.originalFilePath = filepath;
        card.originalContent = content;
        card.title = abcSong->getTitle();
        card.originalTitle = abcSong->getTitle();
        card.titleLines = titleLines;
        card.durationSeconds = abcSong->getDurationSeconds();
        card.instruments = instruments;
        card.order = static_cast<int>(songs_.size());
        card.titleEdited = false;

        songs_.push_back(card);
        return true;
    } catch (...) {
        return false;
    }
}

void SetlistManager::removeSong(size_t index) {
    if (index < songs_.size()) {
        songs_.erase(songs_.begin() + index);

        // Update order values
        for (size_t i = 0; i < songs_.size(); ++i) {
            songs_[i].order = static_cast<int>(i);
        }
    }
}

void SetlistManager::reorderSong(size_t oldIndex, size_t newIndex) {
    if (oldIndex >= songs_.size() || newIndex >= songs_.size() || oldIndex == newIndex) {
        return;
    }

    auto song = songs_[oldIndex];
    songs_.erase(songs_.begin() + oldIndex);
    songs_.insert(songs_.begin() + newIndex, song);

    // Update order values
    for (size_t i = 0; i < songs_.size(); ++i) {
        songs_[i].order = static_cast<int>(i);
    }
}

void SetlistManager::updateSongTitle(size_t index, const std::string& newTitle) {
    if (index < songs_.size()) {
        songs_[index].title = newTitle;
        songs_[index].titleEdited = (newTitle != songs_[index].originalTitle);
    }
}

void SetlistManager::updateTitleLine(size_t songIndex, size_t titleLineIndex, const std::string& newFullTitle) {
    if (songIndex < songs_.size() && titleLineIndex < songs_[songIndex].titleLines.size()) {
        auto& titleLine = songs_[songIndex].titleLines[titleLineIndex];
        titleLine.fullTitle = newFullTitle;
        titleLine.titleEdited = (newFullTitle != titleLine.originalFullTitle);

        // Re-extract instrument from updated title
        std::regex instrumentRegex(R"(\[(.*?)\])");
        std::smatch match;
        if (std::regex_search(newFullTitle, match, instrumentRegex)) {
            std::string potential = match[1].str();
            if (!potential.empty() && potential.find(':') == std::string::npos) {
                titleLine.instrument = potential;
            } else {
                titleLine.instrument = "";
            }
        } else {
            titleLine.instrument = "";
        }

        // Update the instruments display list
        songs_[songIndex].instruments.clear();
        for (const auto& tl : songs_[songIndex].titleLines) {
            if (!tl.instrument.empty()) {
                songs_[songIndex].instruments.push_back(tl.instrument);
            }
        }
    }
}

showtimecalc::domain::Duration SetlistManager::getTotalDuration(int paddingSeconds, int introSeconds) const {
    int totalSeconds = introSeconds;

    for (const auto& song : songs_) {
        totalSeconds += song.durationSeconds;
    }

    // Add padding between songs (N-1 gaps for N songs)
    if (songs_.size() > 1) {
        totalSeconds += paddingSeconds * (static_cast<int>(songs_.size()) - 1);
    }

    return showtimecalc::domain::Duration(totalSeconds);
}

bool SetlistManager::exportToFolder(const std::string& folderPath, bool addNumbering) const {
    try {
        // Create the folder if it doesn't exist
        if (!std::filesystem::exists(folderPath)) {
            std::filesystem::create_directories(folderPath);
        }

        // Copy each file to the destination folder
        for (size_t i = 0; i < songs_.size(); ++i) {
            const auto& song = songs_[i];

            // Create new filename with optional order prefix
            std::string newFilename;
            if (addNumbering) {
                std::string orderPrefix = std::to_string(i + 1);
                if (orderPrefix.length() == 1) {
                    orderPrefix = "0" + orderPrefix;  // Pad with zero: 01, 02, etc.
                }
                newFilename = orderPrefix + "_" + song.filename;
            } else {
                newFilename = song.filename;
            }
            std::string destPath = folderPath + "/" + newFilename;

            // Check if any edits were made
            bool hasEdits = song.titleEdited;
            for (const auto& titleLine : song.titleLines) {
                if (titleLine.titleEdited) {
                    hasEdits = true;
                    break;
                }
            }

            if (hasEdits) {
                // Apply edits to content
                std::string modifiedContent = song.originalContent;

                // Replace T: lines with updated instrument names
                std::istringstream stream(song.originalContent);
                std::ostringstream output;
                std::string line;
                size_t titleLineIndex = 0;
                std::regex titleRegex(R"(^T:\s*(.+))");

                while (std::getline(stream, line)) {
                    std::smatch match;
                    if (std::regex_search(line, match, titleRegex) && titleLineIndex < song.titleLines.size()) {
                        const auto& titleLine = song.titleLines[titleLineIndex];

                        if (titleLine.titleEdited) {
                            // Replace entire T: line with edited full title
                            output << "T:" << titleLine.fullTitle << "\n";
                        } else if (titleLineIndex == 0 && song.titleEdited) {
                            // Replace first title if main title was edited
                            output << "T:" << song.title << "\n";
                        } else {
                            output << line << "\n";
                        }

                        titleLineIndex++;
                    } else {
                        output << line << "\n";
                    }
                }

                // Write modified content
                std::ofstream outFile(destPath);
                if (!outFile.is_open()) {
                    return false;
                }
                outFile << output.str();
                outFile.close();
            } else {
                // Just copy the original file
                std::filesystem::copy_file(song.originalFilePath, destPath,
                                          std::filesystem::copy_options::overwrite_existing);
            }
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> SetlistManager::extractInstruments(const std::string& content) {
    std::vector<std::string> instruments;
    std::istringstream stream(content);
    std::string line;

    // Regex to match T: lines which often contain instrument names in brackets
    std::regex titleRegex(R"(^T:\s*(.+))");
    // Regex to extract text in brackets/parentheses that might be instrument names
    std::regex instrumentRegex(R"(\[(.*?)\]|\((.*?)\))");

    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, titleRegex)) {
            std::string title = match[1].str();

            // Look for instrument names in brackets
            std::sregex_iterator iter(title.begin(), title.end(), instrumentRegex);
            std::sregex_iterator end;

            for (; iter != end; ++iter) {
                std::string potential = (*iter)[1].str();
                if (potential.empty()) {
                    potential = (*iter)[2].str();
                }

                // Filter out time signatures (like "4:22") and keep instrument names
                if (!potential.empty() && potential.find(':') == std::string::npos) {
                    // Check if not already in list
                    if (std::find(instruments.begin(), instruments.end(), potential) == instruments.end()) {
                        instruments.push_back(potential);
                    }
                }
            }
        }

        // Also check %%part-name directive
        std::regex partNameRegex(R"(^%%part-name\s+(.+))");
        if (std::regex_search(line, match, partNameRegex)) {
            std::string partName = match[1].str();
            if (std::find(instruments.begin(), instruments.end(), partName) == instruments.end()) {
                instruments.push_back(partName);
            }
        }
    }

    // If no instruments found, add "Unknown"
    if (instruments.empty()) {
        instruments.push_back("Unknown");
    }

    return instruments;
}

std::vector<TitleLine> SetlistManager::extractTitleLines(const std::string& content) {
    std::vector<TitleLine> titleLines;
    std::istringstream stream(content);
    std::string line;

    // Regex to match T: lines
    std::regex titleRegex(R"(^T:\s*(.+))");
    // Regex to extract text in brackets that might be instrument names
    std::regex instrumentRegex(R"(\[(.*?)\])");

    while (std::getline(stream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, titleRegex)) {
            std::string titleContent = match[1].str();

            TitleLine titleLine;
            titleLine.fullTitle = titleContent;
            titleLine.originalFullTitle = titleContent;
            titleLine.instrument = "";
            titleLine.titleEdited = false;

            // Look for instrument name in brackets
            std::smatch instMatch;
            if (std::regex_search(titleContent, instMatch, instrumentRegex)) {
                std::string potential = instMatch[1].str();

                // Filter out time signatures (like "4:22") and keep instrument names
                if (!potential.empty() && potential.find(':') == std::string::npos) {
                    titleLine.instrument = potential;
                }
            }

            titleLines.push_back(titleLine);
        }
    }

    return titleLines;
}

} // namespace setlistgui
