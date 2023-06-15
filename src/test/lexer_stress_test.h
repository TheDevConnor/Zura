#include <iostream>
#include <fstream>
#include <chrono>

#include "../parser/lexer/lexer.h"

std::string generateStressTestInput(int numTokens) {
    std::string input;

    // Print out the number of tokens scanned
    std::cout << "Number of tokens scanned: " << numTokens << std::endl;

    for (int i = 0; i < numTokens; i++) {
        int tokenType = rand() % 5; // Generate a random token type (0 to 4)

        switch (tokenType) {
            case 0: // Generate identifier
                input += "identifier" + std::to_string(i);
                break;
            case 1: // Generate number
                input += std::to_string(i);
                break;
            case 2: // Generate string
                input += "\"string" + std::to_string(i) + "\"";
                break;
            case 3: // Generate whitespace
                input += " ";
                break;
            case 4: // Generate other special characters
                input += "+-*/(){};,.#!=><:";
                break;
        }

        // Add whitespace between tokens
        input += " ";
    }

    return input;
}

void runStressTest(const std::string& input) {
    init_tokenizer(input.c_str());

    // Start the timer
    auto startTime = std::chrono::high_resolution_clock::now();

    while (true) {
        Token token = scan_token();

        if (token.kind == EOF_TOKEN) {
            break;
        }
    }

    // Stop the timer and calculate the elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Print the elapsed time
    std::cout << "Stress test completed in " << duration.count() << " milliseconds." << std::endl;
}