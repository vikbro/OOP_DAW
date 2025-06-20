//#include <fstream>      // For std::ofstream, std::ifstream
//#include <string>       // For std::string
//#include <stdexcept>    // For std::runtime_error, std::exception
//#include <vector>       // For std::vector (used by audio classes)
//#include <sstream>      // For std::stringstream
//#include <iostream>     // For std::cout, std::cerr
//#include <cmath>        // For sin() in SineGenerator (already in Generator.hpp but good for clarity if used here)

#include "Silence.hpp"
#include "FileAudio.hpp"
#include "Effect.hpp"
#include "AudioFactory.hpp"
#include "Generators/Generator.hpp"

int main() {
    try {
        // Create a dummy PESEN.txt, as in the original main
        std::ofstream oFile("PESEN.txt");
        if (oFile.is_open()) {
            oFile << "30 " << "4 " << "5 " << 10 << " " << 10 << " " << 10 << " " << 10 << " " << 10;
            oFile.close();
        } else {
            std::cerr << "Failed to open PESEN.txt for writing." << std::endl;
        }

        // Demonstrate Silence module
        std::cout << "Demonstrating Silence object:" << std::endl;
        Silence s(44100, 1.5); // 1.5 seconds of silence at 44100 Hz
//        s.printToStream(std::cout);

        // Load base audio file using AudioFactory
        std::cout << "\nLoading audio file sin.wav..." << std::endl;
        std::stringstream cmdStream("FILE sin.wav");
        Audio *file = nullptr;
        try {
            file = AudioFactory::getInstance().createAudio(cmdStream);
        } catch (const std::exception& e) {
            std::cerr << "Error loading sin.wav through AudioFactory: " << e.what() << std::endl;
            std::cerr << "Please ensure 'sin.wav' exists in the working directory and is a valid WAV file." << std::endl;
            // No delete file here as it might not be allocated or factory might handle it.
            // return 1; // Continue to generator demo
        }

        if (file) { // Only proceed with effects if file was loaded
            std::cout << "Original file (sin.wav) details:" << std::endl;
//            file->printToStream(std::cout);

            // Define Effect Operations
            FadeIn fadeInOp{1.0, file->getSampleRate()};
            FadeOut fadeOutOp{1.0, file->getSampleRate()};
            Normalize normalizeOp(*file, 0.9);

            // Apply chain of effects
            Effect<FadeIn> ef1(file, fadeInOp);
            std::cout << "\nAudio after FadeIn (ef1):" << std::endl;
//            ef1.printToStream(std::cout);

            Effect<FadeOut> ef2(&ef1, fadeOutOp);
            std::cout << "\nAudio after FadeOut (ef2):" << std::endl;
//            ef2.printToStream(std::cout);

            Effect<Normalize> efN(&ef2, normalizeOp);
            std::cout << "\nAudio after Normalize (efN) (normalization based on original file):" << std::endl;
//            efN.printToStream(std::cout);

            FileAudio zap("snd_zap.wav");
            Effect<Normalize> normZap(&zap, normalizeOp);
            try{
                FileAudio saveNorDuet(normZap);
                saveNorDuet.writeWAV("norm_snd_zap.wav");
            }catch(...){

            }


            // --- SAVING EFFECT OUTPUT ---
            std::cout << "\nSaving effected audio (efN) to effected_output.wav..." << std::endl;
            try {
                FileAudio effectedAudio(efN);
                effectedAudio.writeWAV("effected_output.wav");
                std::cout << "Effected audio saved successfully to effected_output.wav." << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error saving effected audio: " << e.what() << std::endl;
            }
            // --- END OF SAVING EFFECT OUTPUT ---

            // Print some samples of an intermediate effect for verification
            std::cout << "\nFirst 20 samples of ef2 (FadeIn then FadeOut):" << std::endl;
            const auto& constEf2 = ef2;
            for (size_t i = 0; i < constEf2.getSampleSize() && i < 20; ++i) {
                std::cout << constEf2[i] << (i == 19 || i == constEf2.getSampleSize() - 1 ? "" : " ");
            }
            if (constEf2.getSampleSize() > 0) std::cout << "..." << std::endl;
        } else {
            std::cerr << "Skipping Effect demonstration as sin.wav could not be loaded." << std::endl;
        }

        // --- DEMONSTRATING GENERATOR CLASS ---
        std::cout << "\n\n--- Demonstrating Generator ---" << std::endl;
        try {
            // Create a SineGenerator operation
            SineGenerator sineOp; // Uses default frequency 440 and sample rate 44100 internally

            // Create GeneratorAudio using this operation.
            // The GeneratorAudio container will also be set to the SineGenerator's sample rate.
            double generatorDuration = 2.0; // 2 seconds of audio
            // Ensure the sample rate used by GeneratorAudio matches the one in sineOp.rate for consistency.
            GeneratorAudio<SineGenerator> sineAudio(sineOp.rate, generatorDuration, sineOp);

            std::cout << "Generated Sine Wave (sineAudio) details:" << std::endl;
//            sineAudio.printToStream(std::cout); // Should show sample rate from sineOp.rate

            // Print first few samples of the generated audio
            std::cout << "First 10 samples of generated sine wave (Frequency: " << sineOp.frequency << " Hz):" << std::endl;
            const auto& constSineAudio = sineAudio; // Use a const reference
            for (size_t i = 0; i < constSineAudio.getSampleSize() && i < 10; ++i) {
                std::cout << constSineAudio[i] << (i == 9 || i == constSineAudio.getSampleSize() - 1 ? "" : " ");
            }
            if (constSineAudio.getSampleSize() > 0) std::cout << "..." << std::endl;

            // Save the generated audio to a file
            std::cout << "\nSaving generated sine audio to generated_sine_output.wav..." << std::endl;
            FileAudio generatedFileAudio(sineAudio); // Create FileAudio from GeneratorAudio
            generatedFileAudio.writeWAV("generated_sine_output.wav");
            std::cout << "Generated sine audio saved successfully to generated_sine_output.wav." << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error during Generator demonstration: " << e.what() << std::endl;
        }
        // --- END OF GENERATOR DEMONSTRATION ---

        if (file) { // Only delete if it was allocated
            delete file;
            file = nullptr;
        }

        std::cout << "\nMain function completed successfully." << std::endl;
        return 0;

    } catch (const std::exception &ex) {
        std::cerr << "\nUnhandled exception in main: " << ex.what() << std::endl;
        // Ensure file is deleted if allocated and an exception occurs after its allocation
        // delete file; // This is problematic here because 'file' might not be valid or already deleted.
        // Better to rely on RAII or smart pointers if exceptions are complex.
        // For this structure, the delete is attempted before end of try or in specific catches.
        return 1;
    } catch (...) {
        std::cerr << "\nUnknown unhandled exception in main." << std::endl;
        return 1;
    }
}
