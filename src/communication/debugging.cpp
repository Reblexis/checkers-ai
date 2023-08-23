

void message(std::string message, bool important = false){
    if(important)
        std::cout << "--------------------------\n";
    std::cout << message << "\n";
    if(important)
        std::cout << "--------------------------\n";
}

// Redo message using to allow for messages in style of: message1<<message2<<integer1<<integer2<<message3<<'\n' etc.
