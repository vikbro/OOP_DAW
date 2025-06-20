/**
 * @file Track.hpp
 * @brief Defines the Track class for managing a sequence of audio segments.
 */

#ifndef DAW_TRACK_HPP
#define DAW_TRACK_HPP

#include "Audio.hpp" // Defines the Audio base class

/**
 * @brief Represents a single track within an audio project.
 *
 * A track consists of a sequence of `Audio` objects (clips or segments).
 * It might also have track-specific properties like volume, pan, effects, etc.
 * (though not all are explicitly defined in this version).
 *
 * @note TODO: Consider whether `Track` itself should also inherit from `Audio`.
 *       This would allow tracks to be treated as composite audio objects,
 *       which could be useful for sub-mixing or hierarchical project structures.
 * @note TODO: Ensure that a `Track` cannot contain another `Track` directly
 *       if the design intends a flat list of audio clips per track. If hierarchical
 *       tracks are desired, this needs careful design.
 */
class Track {
private:
    std::vector<Audio *> audio; ///< A collection of pointers to Audio objects forming the track.
    ///< Manages ownership of these Audio objects.
    size_t audioSize;           ///< The number of Audio objects currently in the track.
    ///< This might be redundant if `audio.size()` is always used.

    double frequency;           ///< The sample rate (frequency in Hz) for this track.
    ///< This could influence how audio data is processed or mixed within the track.

public:
    /**
     * @brief Applies one or more audio effects to the track or its constituent audio clips.
     *
     * The specific effects and how they are applied (e.g., to individual clips,
     * the entire track output, or as insert/send effects) would be defined
     * in the implementation. This might involve iterating through audio clips
     * and wrapping them with Effect objects, or applying effects during a mixing process.
     * @note The implementation details (which effects, how they are configured, etc.)
     *       are not specified in this header.
     */
//    void applyEffect();

    // Potential future methods to document if added:
    // Track(double freq);
    // ~Track(); // Important for managing memory of Audio pointers in `audio` vector.
    // void addAudioClip(Audio* clip, size_t position);
    // void removeAudioClip(size_t index);
    // Audio* getAudioClip(size_t index) const;
    // size_t getAudioClipCount() const;
    // void setVolume(double volume);
    // double getVolume() const;
    // void setPan(double pan);
    // double getPan() const;
    // bool isMuted() const;
    // void setMuted(bool muted);
    // bool isSoloed() const;
    // void setSoloed(bool soloed);
    // double getDuration() const; // Would need to calculate total duration from clips
};

#endif //DAW_TRACK_HPP
