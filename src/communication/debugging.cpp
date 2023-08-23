#include <string>
#include <iostream>

void message(std::string message, bool important = false, bool newline = true, bool error = false){
    if(important)
        std::cout << "--------------------------\n";
    std::cout << message;
    if(important || newline)
        std::cout << '\n';
    std::cout<<std::flush;
    if(important)
        std::cout << "--------------------------\n";

    if(error)
        exit(1);
}

// Redo message using to allow for messages in style of: message1<<message2<<integer1<<integer2<<message3<<'\n' etc.
