// Working Code 9/16/2023
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

const int potPinLeft = A0;
const int potPinRight = A1;
const int switchPinLeft = 2;
const int switchPinRight = 3;

const int deadband = 2;

const int maxVolume = 20;

int lastVolume = 0;
int lastPlayedSong = 01;
int lastPlayedFolder = 01;
bool rightSwitchPressed = false;
bool leftSwitchPressed = false;
bool isPlaying = true;

unsigned long lastSwitchPressTimeLeft = 0;
unsigned long lastSwitchPressTimeRight = 0;
const long debounceTime = 50;

bool trackEnded = false;

void setup()
{
    Serial.begin(9600);
    pinMode(potPinLeft, INPUT);
    pinMode(potPinRight, INPUT);
    pinMode(switchPinLeft, INPUT_PULLUP);
    pinMode(switchPinRight, INPUT_PULLUP);

    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial))
    {
        Serial.println("DFPlayer not detected!");
        while (true)
            ;
    }

    myDFPlayer.volume(10);
    myDFPlayer.playFolder(1, 1);
}

void loop()
{
    handleLeftSwitch();
    handleRightSwitch();

    if (myDFPlayer.readType() == DFPlayerPlayFinished && !trackEnded)
    {
        int randomTrack = random(1, myDFPlayer.readFileCountsInFolder(lastPlayedFolder) + 1);
        myDFPlayer.playFolder(lastPlayedFolder, randomTrack);
        trackEnded = true;
    }
}

void handleLeftSwitch()
{
    if (digitalRead(switchPinLeft) == LOW)
    {
        if (millis() - lastSwitchPressTimeLeft > debounceTime)
        {
            if (!leftSwitchPressed)
            {
                Serial.println("Left switch pressed. Volume Control ON");
                leftSwitchPressed = true;
                if (!isPlaying)
                {
                    myDFPlayer.playFolder(lastPlayedFolder, lastPlayedSong);
                    isPlaying = true;
                }
            }

            int rawValue = analogRead(potPinLeft);
            Serial.print("Raw potentiometer value for volume: ");
            Serial.println(rawValue);

            float volumeRatio = float(rawValue) / 1023.0;      // Convert rawValue to a ratio between 0 and 1
            int volume = int(pow(volumeRatio, 2) * maxVolume); // Square the ratio for a logarithmic effect

            volume = constrain(volume, 0, maxVolume);

            if (abs(volume - lastVolume) > deadband)
            {
                myDFPlayer.volume(volume);
                lastVolume = volume;
                Serial.print("Volume set to: ");
                Serial.println(volume);
            }

            lastSwitchPressTimeLeft = millis();
        }
    }
    else
    {
        if (leftSwitchPressed)
        {
            Serial.println("Left switch released. Volume Control OFF");
            leftSwitchPressed = false;
            myDFPlayer.pause();
            isPlaying = false;
        }
    }
}

void handleRightSwitch()
{
    if (digitalRead(switchPinRight) == LOW)
    {
        if (millis() - lastSwitchPressTimeRight > debounceTime)
        {
            if (!rightSwitchPressed)
            {
                Serial.println("Right switch pressed. Changing Folder");
                rightSwitchPressed = true;

                if (lastPlayedFolder < myDFPlayer.readFolderCounts())
                {
                    lastPlayedFolder++;
                }
                else
                {
                    lastPlayedFolder = 1; // Reset to the first folder
                }
                myDFPlayer.playFolder(lastPlayedFolder, 1);
            }

            int rawValue = analogRead(potPinRight);
            Serial.print("Raw potentiometer value for track: ");
            Serial.println(rawValue);

            int trackNumber = map(rawValue, 0, 1023, 1, 30);
            if (abs(trackNumber - lastPlayedSong) > deadband)
            {
                myDFPlayer.playFolder(lastPlayedFolder, trackNumber);
                lastPlayedSong = trackNumber;
                trackEnded = false;
            }

            if (myDFPlayer.readType() == DFPlayerPlayFinished && lastPlayedFolder >= myDFPlayer.readFileCountsInFolder(lastPlayedFolder))
            {
                lastPlayedFolder = 1;
                myDFPlayer.playFolder(lastPlayedFolder, 1);
            }

            lastSwitchPressTimeRight = millis();
        }
    }
    else
    {
        if (rightSwitchPressed)
        {
            Serial.println("Right switch released. Track selection OFF");
            rightSwitchPressed = false;
        }
    }
}
