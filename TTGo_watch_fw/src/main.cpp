#include "config.h"

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

// Watch objects
TTGOClass *watch;
TFT_eSPI *tft;
BMA *sensor;

uint32_t sessionId = 30;

volatile uint8_t state;
volatile bool irqBMA = false;
volatile bool irqButton = false;

bool sessionStored = false;
bool sessionSent = false;

uint32_t steps = 0;
float dist = 0;
unsigned long sessionStartTime = 0;
uint32_t elapsedTime = 0;

void initHikeWatch()
{
    // LittleFS
    if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

    // Stepcounter
    // Configure IMU
    Acfg cfg;
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    cfg.range = BMA4_ACCEL_RANGE_2G;
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;
    sensor->accelConfig(cfg);
    sensor->enableAccel();

    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);
    // Reset steps
    sensor->resetStepCounter();
    // Turn on step interrupt
    sensor->enableStepCountInterrupt();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        // Set interrupt to set irqBMA value to 1
        irqBMA = 1;
    }, RISING); 

    // Side button
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irqButton = true;
    }, FALLING);

    //!Clear IRQ unprocessed first
    watch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    watch->power->clearIRQ();

    return;
}

void sendDataBT(fs::FS &fs, const char * path)
{
    /* Sends data via SerialBT */
    fs::File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.println("- read from file:");
    while(file.available()){
        SerialBT.write(file.read());
    }
    file.close();
}

void sendSessionBT()
{
    // Read session and send it via SerialBT
    watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
    watch->tft->drawString("Sending session", 20, 80);
    watch->tft->drawString("to Hub", 80, 110);

    // Sending session id
    sendDataBT(LITTLEFS, "/id.txt");
    SerialBT.write(';');
    // Sending steps
    sendDataBT(LITTLEFS, "/steps.txt");
    SerialBT.write(';');
    // Sending distance
    sendDataBT(LITTLEFS, "/distance.txt");
    SerialBT.write(';');
    // Sending session time
    sendDataBT(LITTLEFS, "/sessiontime.txt");
    SerialBT.write(';');
    // Send connection termination char
    SerialBT.write('\n');
}


void saveIdToFile(uint16_t id)
{
    char buffer[10];
    itoa(id, buffer, 10);
    writeFile(LITTLEFS, "/id.txt", buffer);
}

void saveStepsToFile(uint32_t step_count)
{
    char buffer[10];
    itoa(step_count, buffer, 10);
    writeFile(LITTLEFS, "/steps.txt", buffer);
}

void saveDistanceToFile(float distance)
{
    char buffer[10];
    itoa(distance, buffer, 10);
    writeFile(LITTLEFS, "/distance.txt", buffer);
}

void saveSessiontimeToFile(uint32_t elapsedTime)
{
    char buffer[10];
    itoa(elapsedTime, buffer, 10);
    writeFile(LITTLEFS, "/sessiontime.txt", buffer);
}

void deleteSession()
{
    deleteFile(LITTLEFS, "/id.txt");
    deleteFile(LITTLEFS, "/distance.txt");
    deleteFile(LITTLEFS, "/steps.txt");
    //deleteFile(LITTLEFS, "/coord.txt");
    deleteFile(LITTLEFS, "/sessiontime.txt");
}

void setup()
{
    Serial.begin(115200);
    watch = TTGOClass::getWatch();
    watch->begin();
    watch->openBL();
    //Receive objects for easy writing
    tft = watch->tft;
    sensor = watch->bma;
    
    initHikeWatch();

    state = 1;

    SerialBT.begin("Hiking Watch");
}

void loop()
{
    switch (state)
    {
    case 1:
    {
        /* Initial stage */
        //Basic interface
        watch->tft->fillScreen(TFT_BLACK);  
        watch->tft->setTextFont(4);
        watch->tft->setTextColor(TFT_WHITE, TFT_BLACK);
        watch->tft->drawString("Hiking Watch",  45, 25, 4);
        watch->tft->drawString("Press button", 50, 80);
        watch->tft->drawString("to start session", 40, 110);

        bool exitSync = false;

        //Bluetooth discovery
        while (1)
        {
            /* Bluetooth sync */
            if (SerialBT.available())
            {
                char incomingChar = SerialBT.read();
                if (incomingChar == 'c' and sessionStored and not sessionSent)
                {
                    sendSessionBT();
                    sessionSent = true;
                    watch->tft->fillScreen(TFT_BLACK);
                    watch->tft->drawString("Data sent",  45, 25, 4);
                    delay(1000);
                }

                if (sessionSent && sessionStored) {
                    // Update timeout before blocking while
                    unsigned long updateTimeout = 0;
                    unsigned long last = millis();
                    while(1)
                    {
                        updateTimeout = millis();

                        if (SerialBT.available())
                            incomingChar = SerialBT.read();
                        if (incomingChar == 'r')
                        {
                            Serial.println("Got an R");
                            // Delete session
                            deleteSession();
                            sessionStored = false;
                            sessionSent = false;
                            incomingChar = 'q';
                            exitSync = true;
                            break;
                        }
                        else if ((millis() - updateTimeout > 2000))
                        {
                            Serial.println("Waiting for timeout to expire");
                            updateTimeout = millis();
                            sessionSent = false;
                            exitSync = true;
                            break;
                        }
                    }
                }
            }
            if (exitSync)
            {
                delay(1000);
                watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                watch->tft->drawString("Hiking Watch",  45, 25, 4);
                watch->tft->drawString("Press button", 50, 80);
                watch->tft->drawString("to start session", 40, 110);
                exitSync = false;
            }

            /*      IRQ     */
            if (irqButton) {
                irqButton = false;
                watch->power->readIRQ();
                if (state == 1)
                {
                    state = 2;
                }
                watch->power->clearIRQ();
            }
            if (state == 2) {
                if (sessionStored)
                {
                    watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                    watch->tft->drawString("Overwriting",  55, 100, 4);
                    watch->tft->drawString("session", 70, 130);
                    delay(1000);
                }
                break;
            }
        }
        break;
    }
    case 2:
    {
        /* Hiking session initalisation */
        sensor->begin();
        sessionStartTime = millis();
        state = 3;
        break;
    }
    case 3:
    {
        /* Hiking session ongoing */

        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->drawString("Starting hike", 45, 100);
        delay(1000);
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);

        watch->tft->setCursor(45, 70);
        watch->tft->print("Steps: 0");

        watch->tft->setCursor(45, 100);
        watch->tft->print("Dist: 0 m");

        watch->tft->setCursor(45, 130);
        watch->tft->print("Time: 0 sec");

        bool confirmReset = false;
        unsigned long confirmationStartTime = millis();
        
        while(1){
            if (irqButton) {
                irqButton = false;
                if (!confirmReset) {
                    watch->power->clearIRQ();
                    confirmReset = true;
                    confirmationStartTime = millis();
            } else {
                // Break out of the loop and store data
                break;
            }    
            } else if (irqBMA && !confirmReset) {
                irqBMA = false;
                steps = sensor->getCounter();
                dist  = steps*0.76 ;   // distance in meters
                elapsedTime = (millis() - sessionStartTime) / 1000; // Convert milliseconds to seconds

                watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
                watch->tft->setTextColor(random(0xFFFF), TFT_BLACK);
                watch->tft->setCursor(45, 70);
                watch->tft->print("Steps:");
                watch->tft->print(steps);
                
                watch->tft->setCursor(45, 100);
                watch->tft->print("Dist:");
                if (dist < 1000){
                    watch->tft->print(dist);
                    watch->tft->print("m");
                } else if (dist >= 1000){
                    watch->tft->print(dist/1000);
                    watch->tft->print("km");   
                }

                watch->tft->setCursor(45, 130);
                watch->tft->print("Time: ");
                if (elapsedTime < 60){
                    watch->tft->print(elapsedTime);
                    watch->tft->print(" sec");
                } else if (elapsedTime >= 60){
                    watch->tft->print(elapsedTime/60);
                    watch->tft->print(" min");
                }

                Serial.print("Steps: ");
                Serial.print(steps);
                Serial.print(" Dist: ");
                Serial.print(dist);
                Serial.print(" Time: ");
                Serial.println(elapsedTime);
            }
            if (confirmReset) {
                unsigned long elapsedConfirmationTime = millis() - confirmationStartTime;
                if (elapsedConfirmationTime <= 5000) { // Countdown time (in milliseconds)
                    // Display countdown
                    int remainingSeconds = (5000 - elapsedConfirmationTime) / 1000;
                    watch->tft->setCursor(45, 160);
                    watch->tft->print("Confirm reset in ");
                    watch->tft->setCursor(100, 190);
                    watch->tft->print(remainingSeconds);
                    watch->tft->print(" s");

                    Serial.print("Confirm reset in ");
                    Serial.println(remainingSeconds);
                    // Reset confirmReset flag after countdown finishes
                }else {  
                        confirmReset = false;
                        Serial.println("Continue step counting ");
                }
            }
        }
        irqButton = false;
        watch->power->clearIRQ();

        unsigned long last = millis();
        unsigned long updateTimeout = 0;

        //reset step-counter
        sensor->resetStepCounter();
        state=4;
        break;
    }
    case 4:
    {
        //Save hiking session data
        saveIdToFile(sessionId);
        saveStepsToFile(steps);
        saveDistanceToFile(dist);
        saveSessiontimeToFile(elapsedTime);
        sessionStored = true;
        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);
        watch->tft->drawString("Session stored",  45, 100);
        delay(1000);
        state = 1;  
        break;
    }
    default:
        // Restart watch
        ESP.restart();
        break;
    }
}