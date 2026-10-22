#ifndef SERIAL_CONSOLE_H
#define SERIAL_CONSOLE_H

/**
 * @file SerialConsole.h
 * @brief Reads the commands typed in the Serial Monitor ("New Line" ending, 115200 baud).
 *
 * Accepted commands (upper or lower case):
 *   G=2800    gas base value (raw ADC 0..4095)
 *   T=55      temperature base value (Celsius)
 *   M=1 / M=0 motion on / off
 *   SILENCE / UNSILENCE   mute or unmute the buzzer (simulated mobile app)
 *   HELP      prints the list again
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/** @brief Kind of command typed by the user. */
enum class ConsoleCommandType { NONE, SET_GAS, SET_TEMPERATURE, SET_MOTION, SILENCE, UNSILENCE, HELP, UNKNOWN };

/** @brief One parsed command: its kind and, for G=/T=/M=, its number. */
struct ConsoleCommand {
    ConsoleCommandType type;
    float value;
};

/**
 * @class SerialConsole
 * @brief Turns a typed line into a ConsoleCommand.
 */
class SerialConsole {
public:
    /**
     * @brief Checks whether a full line arrived and which command it is.
     * @return The command, or type NONE when nothing arrived.
     */
    ConsoleCommand poll() {
        ConsoleCommand command = {ConsoleCommandType::NONE, 0.0f};
        if (!Serial.available()) return command;
        String typedText = Serial.readStringUntil(LINE_END);
        typedText.trim();
        typedText.toUpperCase();
        if (typedText.isEmpty()) return command;

        command.type = ConsoleCommandType::UNKNOWN;
        for (const CommandSyntax& syntax : COMMAND_TABLE) {
            if (syntax.hasValue && typedText.startsWith(syntax.text)) {
                command.type = syntax.type;
                command.value = typedText.substring(strlen(syntax.text)).toFloat();
                break;
            }
            if (!syntax.hasValue && typedText.equals(syntax.text)) {
                command.type = syntax.type;
                break;
            }
        }
        if (command.type == ConsoleCommandType::UNKNOWN) {
            Serial.printf("[SIM] Comando desconocido: %s (escribe HELP)\n", typedText.c_str());
        }
        return command;
    }

private:
    static constexpr char LINE_END = '\n';

    struct CommandSyntax {
        const char* text;
        ConsoleCommandType type;
        bool hasValue;
    };
    static constexpr CommandSyntax COMMAND_TABLE[] = {
        {"G=", ConsoleCommandType::SET_GAS, true},
        {"T=", ConsoleCommandType::SET_TEMPERATURE, true},
        {"M=", ConsoleCommandType::SET_MOTION, true},
        {"SILENCE", ConsoleCommandType::SILENCE, false},
        {"UNSILENCE", ConsoleCommandType::UNSILENCE, false},
        {"HELP", ConsoleCommandType::HELP, false},
    };
};

#endif // SERIAL_CONSOLE_H
