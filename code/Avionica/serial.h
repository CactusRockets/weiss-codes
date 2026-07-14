template<typename U>
void print(U variable) {
    if(ENABLE_SERIAL) {
        Serial.print(variable);
    }
}

template<typename N>
void println(N variable) {
    if(ENABLE_SERIAL) {
        Serial.println(variable);
    }
}

template<typename U, typename... Args>
void print(U variable, Args... args) {
    if(ENABLE_SERIAL) {
        Serial.print(variable, args...);
    }
}

template<typename N, typename... Args>
void println(N variable, Args... args) {
    if(ENABLE_SERIAL) {
        Serial.println(variable, args...);
    }
}