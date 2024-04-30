#ifndef DEBUGGING_HPP
#define DEBUGGING_HPP

#include <string>

/**
 * @brief Prints a message to the console.
 * Allows for simpler control of debugging in the project.
 *
 * @param message The message to print
 * @param important Whether the message is important (controls formatting)
 * @param newline Whether to print a newline after the message
 * @param error Whether the message is an error message
 */
void message(std::string message, bool important = false, bool newline = true, bool error = false);

#endif