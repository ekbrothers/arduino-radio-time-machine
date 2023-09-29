// Testing Framework

/////////////////////////////Potentiometer Switch Testing/////////////////////////////////////////

// #include <SoftwareSerial.h>
// #include <DFRobotDFPlayerMini.h>

// SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
// DFRobotDFPlayerMini myDFPlayer;
// const int switchPinLeft = 2;
// const int switchPinRight = 3;

// void setup()
// {
//     Serial.begin(9600);
//     pinMode(switchPinLeft, INPUT_PULLUP);
//     pinMode(switchPinRight, INPUT_PULLUP);
// }
// void loop()
// {
//     bool leftSwitchState = digitalRead(switchPinLeft) == LOW;
//     bool rightSwitchState = digitalRead(switchPinRight) == LOW;

// Code for testing left and right switches
//     Serial.print("Left Switch State: ");
//     Serial.println(leftSwitchState ? "Pressed" : "Not Pressed");
//     Serial.print("Right Switch State: ");
//     Serial.println(rightSwitchState ? "Pressed" : "Not Pressed");

//     delay(500); // delay to make the output readable, adjust as necessary
// }
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////Potentiometer Dial Testing//////////////////////////////////
// const int potPin1 = A0; // Assume you have a potentiometer connected to A0
// const int potPin2 = A1; // And another to A1, Add more as needed

// void setup() {
//   Serial.begin(9600);
// }

// void loop() {
//   int val1 = analogRead(potPin1);
//   int val2 = analogRead(potPin2);
//   Serial.print("Pot1: ");
//   Serial.print(val1);
//   Serial.print("\tPot2: ");
//   Serial.println(val2);

//   delay(100); // Add a delay to make the output readable, adjust as needed
// }
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////Dfplayer Testing///////////////////////////////////////////
// #include <Arduino.h>
// #include <DFRobotDFPlayerMini.h>
// #include <SoftwareSerial.h>

// SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
// DFRobotDFPlayerMini myDFPlayer;

// void setup() {
//   mySoftwareSerial.begin(9600);
//   Serial.begin(115200);

//   if (!myDFPlayer.begin(mySoftwareSerial)) {
//     Serial.println(F("Error: Unable to begin DFPlayer Mini"));
//     while (true);
//   }

//   myDFPlayer.volume(10); // Set volume level (0-30)
//   myDFPlayer.play(1);    // Play the first track
// }

// void loop() {
// }
////////////////////////////////////////////////////////////////////////////////////////

// Working Code
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <Button.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

const int potPinLeft = A0;
const int potPinRight = A1;
Button leftPauseSwitch(2);    // Left Pause Switch
Button rightShuffleSwitch(3); // Right Shuffle Switch

const int maxVolume = 30;
int prevVolume = 0;
byte volumeLevel = 0;

// Update this number for the number of folders
const int folderCount = 5;

int lastPlayedSong = 01;
int lastPlayedFolder = 01;
bool isPlaying = true;
bool trackEnded = false;

void setup()
{
    Serial.begin(9600);
    pinMode(potPinLeft, INPUT);
    pinMode(potPinRight, INPUT);
    leftPauseSwitch.begin();
    rightShuffleSwitch.begin();
    mySoftwareSerial.begin(9600);

    if (!myDFPlayer.begin(mySoftwareSerial))
    {
        while (true)
            ;
    }

    volumeLevel = map(analogRead(potPinLeft), 0, 1023, 0, maxVolume);
    myDFPlayer.volume(volumeLevel);
    prevVolume = volumeLevel;

    myDFPlayer.playFolder(1, 1);
}

void loop()
{
    handleLeftPauseSwitch();
    handleRightShuffleSwitch();

    volumeLevel = map(analogRead(potPinLeft), 0, 1023, 0, maxVolume);

    if (abs(volumeLevel - prevVolume) >= 3)
    {
        myDFPlayer.volume(volumeLevel);
        Serial.print("Current Volume Level: ");
        Serial.println(volumeLevel);
        prevVolume = volumeLevel;
        delay(1);
    }

    if (myDFPlayer.readType() == DFPlayerPlayFinished && !trackEnded)
    {
        int randomTrack = random(1, myDFPlayer.readFileCountsInFolder(lastPlayedFolder) + 1);
        Serial.print("Playing Random Track: "); // Output the track being played.
        Serial.println(randomTrack);            // Output the chosen random track number.
        myDFPlayer.playFolder(lastPlayedFolder, randomTrack);
        trackEnded = true;
    }
}

void handleLeftPauseSwitch()
{
    if (leftPauseSwitch.pressed())
    {
        if (!isPlaying)
        {
            myDFPlayer.playFolder(lastPlayedFolder, lastPlayedSong);
            isPlaying = true;
        }
    }
    if (leftPauseSwitch.released())
    {
        myDFPlayer.pause();
        isPlaying = false;
    }
}

void handleRightShuffleSwitch()
{
    if (rightShuffleSwitch.pressed())
    {
        Serial.print("Number of Folders: ");
        // Serial.print ln(myDFPlayer.readFolderCounts());
        Serial.println("Switch Pressed");
        Serial.print("Last Played Folder Before Increment: ");
        Serial.println(lastPlayedFolder);

        if (lastPlayedFolder < folderCount)
        {
            // if (lastPlayedFolder < myDFPlayer.readFolderCounts()) {
            lastPlayedFolder++;
        }
        else
        {
            lastPlayedFolder = 1;
        }
        Serial.print("Last Played Folder After Increment: ");
        Serial.println(lastPlayedFolder);
        myDFPlayer.playFolder(lastPlayedFolder, 1);
    }
    if (rightShuffleSwitch.read() == Button::PRESSED)
    {
        int rawValue = analogRead(potPinRight);
        int trackNumber = map(rawValue, 0, 1023, 1, 50);
        if (abs(trackNumber - lastPlayedSong) > 2)
        {
            Serial.print("Playing Track: "); // Output the track being played.
            Serial.println(trackNumber);     // Output the calculated track number.
            myDFPlayer.playFolder(lastPlayedFolder, trackNumber);
            lastPlayedSong = trackNumber;
            trackEnded = false;
        }
    }
}
