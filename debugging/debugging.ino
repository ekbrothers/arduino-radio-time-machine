// Audio Player Debugging
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX (change these pins if needed)
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
    Serial.begin(9600);

    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial))
    {
        Serial.println("DFPlayer not detected!");
        while (true)
            ; // halt if module is not connected properly
    }

    Serial.println("DFPlayer ready.");

    myDFPlayer.volume(10); // Set initial volume
    myDFPlayer.play(1);    // Play the first song
}

void loop()
{
    delay(5000);           // Wait for 5 seconds
    myDFPlayer.volumeUp(); // Increase the volume

    delay(5000);       // Wait for 5 more seconds
    myDFPlayer.next(); // Play the next song

    delay(5000);        // Wait yet another 5 seconds
    myDFPlayer.play(1); // Play the first song again
}

Simpler MP #player Test
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

    SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
    Serial.begin(9600);
    mySoftwareSerial.begin(9600);

    Serial.println("Initializing DFPlayer ...");

    if (!myDFPlayer.begin(mySoftwareSerial))
    { // Use softwareSerial to communicate with DFPlayer
        Serial.println("Unable to begin:");
        Serial.println("1. Check the connection.");
        Serial.println("2. Insert the SD card.");
        while (true)
            ;
    }

    Serial.println("DFPlayer Mini ready.");

    // Print some information about the module and SD card
    Serial.print("Total file count: ");
    Serial.println(myDFPlayer.readFileCounts()); // Get all file counts in SD card

    Serial.print("Total folder count: ");
    Serial.println(myDFPlayer.readFolderCounts()); // Get all folder counts in SD card

    myDFPlayer.volume(40); // Set volume value (0~30).
    myDFPlayer.play(1);    // Play track number 1
}

void loop()
{
    if (myDFPlayer.available())
    {
        printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer
    }
}

void printDetail(uint8_t type, int value)
{
    switch (type)
    {
    case TimeOut:
        Serial.println("Time Out!");
        break;
    case WrongStack:
        Serial.println("Stack Wrong!");
        break;
    case DFPlayerCardInserted:
        Serial.println("Card Inserted!");
        break;
    case DFPlayerCardRemoved:
        Serial.println("Card Removed!");
        break;
    case DFPlayerCardOnline:
        Serial.println("Card Online!");
        break;
    case DFPlayerPlayFinished:
        Serial.print("Track ");
        Serial.print(value);
        Serial.println(" Play Finished!");
        break;
    case DFPlayerError:
        Serial.print("DFPlayerError: ");
        switch (value)
        {
        case Busy:
            Serial.println("Card not found");
            break;
        case Sleeping:
            Serial.println("Sleeping");
            break;
        case SerialWrongStack:
            Serial.println("Get Wrong Stack");
            break;
        case CheckSumNotMatch:
            Serial.println("Check Sum Not Match");
            break;
        case FileIndexOut:
            Serial.println("File Index Out of Bound");
            break;
        case FileMismatch:
            Serial.println("Cannot Find File");
            break;
        case Advertise:
            Serial.println("In Advertise");
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}