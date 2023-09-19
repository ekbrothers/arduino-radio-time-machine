// Include necessary libraries
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>

// Configure SoftwareSerial for communication
SoftwareSerial mySoftwareSerial(10, 11); // Define RX and TX pins
DFRobotDFPlayerMini myDFPlayer;          // Create DFPlayer instance

// Define constants
const int DEADBAND = 2;
const int MAX_VOLUME = 20;
const long DEBOUNCE_TIME = 50;

// Define a structure to encapsulate switch properties
struct Switch
{
    int pin;                     // Switch's pin number
    bool pressed;                // Flag to track switch's state
    unsigned long lastPressTime; // Last time the switch was pressed
};

// Instances of the Switch structure for left and right switches
Switch leftSwitch = {2, false, 0};
Switch rightSwitch = {3, false, 0};

// Define a structure to encapsulate potentiometer properties
struct Potentiometer
{
    int pin;       // Potentiometer's pin number
    int lastValue; // Last known value of the potentiometer
};

// Instances of the Potentiometer structure for left and right potentiometers
Potentiometer leftPot = {A0, 0};
Potentiometer rightPot = {A1, 0};

// Variables to track the player state
int lastPlayedSong = 01;
int lastPlayedFolder = 01;
bool isPlaying = true;
bool trackEnded = false;

// Setup function initializes configurations
void setup()
{
    wdt_disable(); // Disable the watchdog timer during setup to prevent unintentional resets

    Serial.begin(9600); // Start serial communication

    // Set pin modes for switches and potentiometers
    pinMode(leftPot.pin, INPUT);
    pinMode(rightPot.pin, INPUT);
    pinMode(leftSwitch.pin, INPUT_PULLUP);
    pinMode(rightSwitch.pin, INPUT_PULLUP);

    // Initialize communication with DFPlayer
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) // If initialization fails
    {
        Serial.println("DFPlayer not detected!"); // Notify via Serial
        while (true)
        {
        } // Halt further execution
    }

    // Set initial volume and start playing the first track in the first folder
    myDFPlayer.volume(10);
    myDFPlayer.playFolder(1, 1);

    // Enable the watchdog timer with a timeout of 8 seconds
    wdt_enable(WDTO_8S);
}

// Main loop function
void loop()
{
    wdt_reset(); // Reset the watchdog timer to avoid unintentional resets

    // Handle switch presses and potentiometer readings
    handleSwitch(leftSwitch, isPlaying, leftPot);
    handleSwitch(rightSwitch, isPlaying, rightPot);

    // Play a random track when the current track finishes
    if (myDFPlayer.readType() == DFPlayerPlayFinished && !trackEnded)
    {
        int randomTrack = random(1, myDFPlayer.readFileCountsInFolder(lastPlayedFolder) + 1);
        myDFPlayer.playFolder(lastPlayedFolder, randomTrack);
        trackEnded = true;
    }
}

// Function to handle switch presses and releases
void handleSwitch(Switch &sw, bool &isPlayingFlag, Potentiometer &pot)
{
    if (digitalRead(sw.pin) == LOW) // If the switch is pressed
    {
        // Debounce the switch press
        if (millis() - sw.lastPressTime > DEBOUNCE_TIME)
        {
            // Execute if this is the first time the switch is recognized as pressed
            if (!sw.pressed)
            {
                Serial.println((sw.pin == leftSwitch.pin) ? "Left switch pressed. Volume Control ON" : "Right switch pressed. Changing Folder");
                sw.pressed = true; // Mark the switch as pressed

                // Handle actions specific to the left or right switch
                if (sw.pin == leftSwitch.pin && !isPlayingFlag)
                {
                    // If playback was paused, resume it
                    myDFPlayer.playFolder(lastPlayedFolder, lastPlayedSong);
                    isPlayingFlag = true;
                }
                else if (sw.pin == rightSwitch.pin)
                {
                    // Change to the next folder, or wrap around to the first folder
                    if (lastPlayedFolder < myDFPlayer.readFolderCounts())
                    {
                        lastPlayedFolder++;
                    }
                    else
                    {
                        lastPlayedFolder = 1;
                    }
                    myDFPlayer.playFolder(lastPlayedFolder, 1); // Play the first track of the selected folder
                }
            }

            // Handle potentiometer readings based on which switch was pressed
            int rawValue = analogRead(pot.pin);
            if (sw.pin == leftSwitch.pin)
            {
                handleLeftPotentiometer(rawValue);
            }
            else
            {
                handleRightPotentiometer(rawValue);
            }

            sw.lastPressTime = millis(); // Update the last recognized press time
        }
    }
    else // If the switch is not pressed
    {
        // Execute if this is the first time the switch is recognized as not pressed
        if (sw.pressed)
        {
            Serial.println((sw.pin == leftSwitch.pin) ? "Left switch released. Volume Control OFF" : "Right switch released. Track selection OFF");
            sw.pressed = false; // Mark the switch as not pressed

            // Pause playback if the left switch was released
            if (sw.pin == leftSwitch.pin)
            {
                myDFPlayer.pause();
                isPlayingFlag = false;
            }
        }
    }
}

// Function to handle volume control using the left potentiometer
void handleLeftPotentiometer(int rawValue)
{
    Serial.print("Raw potentiometer value for volume: ");
    Serial.println(rawValue);

    // Convert raw potentiometer value to a volume level
    float volumeRatio = float(rawValue) / 1023.0;
    int volume = int(pow(volumeRatio, 2) * MAX_VOLUME);
    volume = constrain(volume, 0, MAX_VOLUME);

    // If the change in volume exceeds the deadband
    if (abs(volume - leftPot.lastValue) > DEADBAND)
    {
        myDFPlayer.volume(volume);  // Adjust the volume
        leftPot.lastValue = volume; // Update the last known volume

        Serial.print("Volume set to: ");
        Serial.println(volume); // Notify the user via Serial
    }
}

// Function to handle track selection using the right potentiometer
void handleRightPotentiometer(int rawValue)
{
    Serial.print("Raw potentiometer value for track: ");
    Serial.println(rawValue);

    // // Convert raw potentiometer value to a track number within the range of tracks in the current folder

    int trackNumber = map(rawValue, 0, 1023, 1, 30);

    if (abs(trackNumber - rightPot.lastValue) > DEADBAND)
    {
        myDFPlayer.playFolder(lastPlayedFolder, trackNumber); // Play the selected track
        rightPot.lastValue = trackNumber;                     // Update the last known track number
        trackEnded = false;
    }

    // Play the first track of the first folder if the last track of the last folder has finished playing
    if (myDFPlayer.readType() == DFPlayerPlayFinished && lastPlayedFolder >= myDFPlayer.readFileCountsInFolder(lastPlayedFolder))
    {
        lastPlayedFolder = 1;
        myDFPlayer.playFolder(lastPlayedFolder, 1);
    }
}
