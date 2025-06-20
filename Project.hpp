/**
 * @file Project.hpp
 * @brief Defines the Project class for managing a collection of audio tracks.
 */

#ifndef DAW_PROJECT_HPP
#define DAW_PROJECT_HPP

#include "Track.hpp" // Assumes Track.hpp defines the Track class

/**
 * @brief Represents an audio project, which consists of multiple tracks.
 *
 * The Project class manages a collection of `Track` objects and provides
 * functionalities for creating, saving, and loading projects.
 */
class Project {
private:
    std::vector<Track> tracks; ///< A collection of tracks within the project.

public:
    /**
     * @brief Constructs a new Project.
     * @param frequency The master sample rate for the project (e.g., 44100 Hz).
     *                  This might be used to initialize tracks or project-wide settings.
     * @param trackAmount The initial number of tracks to create in the project.
     *                    Tracks might be initialized with default settings based on the project's frequency.
     */
    Project(double frequency, int trackAmount);

    /**
     * @brief Creates a new project with a given name.
     *
     * This static factory method might handle setting up a default project structure
     * or preparing storage associated with the project name.
     * @param name The name for the new project. This could be a file path or an identifier.
     * @return A Project object initialized for a new project.
     * @note The implementation details (e.g., if it creates files on disk immediately) are not defined here.
     */
    static Project createNew(const char* name);

    /**
     * @brief Saves the current state of the project.
     * @param projectName The name (often a file path) to which the project should be saved.
     *                    This method would serialize all tracks and project-specific data.
     */
    void save(const char* projectName);

    /**
     * @brief Loads a project from a specified location.
     * @param projectName The name (often a file path) from which to load the project.
     *                    This method would deserialize project data and reconstruct the tracks.
     * @note The parameter type `const char& projectName` seems unusual for a C-style string (filename)
     *       and is likely intended to be `const char* projectName`. This should be reviewed.
     */
    void load(const char& projectName); // TODO: Review parameter type, likely should be const char*
};

#endif //DAW_PROJECT_HPP
